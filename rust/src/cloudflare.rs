use std::net::IpAddr;

use crate::{GetIpVersion, Result};
use serde_json::Value;

use crate::config::Config;

#[derive(Debug, serde::Deserialize)]
pub struct ApiResponse<T> {
    pub success: bool,
    pub result: Option<T>,
    pub errors: Vec<Value>,
    pub messages: Vec<Message>,
}

impl<T> ApiResponse<T> {
    pub fn solve(self) -> Result<T> {
        for message in self.messages {
            log::info!("code: {}, message: {}", message.code, message.message);
        }
        if self.success {
            Ok(self.result.unwrap())
        } else {
            Err(Value::Array(self.errors).to_string())?
        }
    }
}

#[derive(Debug, serde::Deserialize)]
pub struct Message {
    pub code: usize,
    pub message: String,
}

#[derive(Debug, serde::Deserialize)]
pub struct VerifyApi {}

#[derive(Debug, serde::Deserialize)]
pub struct Zone {
    pub name: String,
    pub id: String,
}

#[derive(Debug)]
pub struct Zones {
    zones: Vec<Zone>,
}

impl std::fmt::Display for Zones {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Debug::fmt(&self.zones, f)
    }
}

impl<'de> serde::Deserialize<'de> for Zones {
    fn deserialize<D>(deserializer: D) -> std::result::Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        Vec::<Zone>::deserialize(deserializer).map(|zones| Zones { zones })
    }
}

trait SetHeader {
    fn set_header(self, config: &Config) -> Self;
}

impl SetHeader for reqwest::RequestBuilder {
    fn set_header(self, config: &Config) -> Self {
        let builder = self.header("X-Auth-Email", &config.header.email);
        if config.header.api_key_type == "Auth" {
            builder.header("X-Auth-Key", &config.header.api_key)
        } else {
            let val = &format!("Bearer {}", &config.header.api_key);
            builder.header("Authorization", val)
        }
    }
}

pub async fn verify_api(client: &reqwest::Client, config: &Config) -> Result<VerifyApi> {
    client
        .get("https://api.cloudflare.com/client/v4/user/tokens/verify")
        .set_header(config)
        .send()
        .await?
        .json::<ApiResponse<VerifyApi>>()
        .await?
        .solve()
}

pub async fn find_zone_id(client: &reqwest::Client, config: &Config) -> Result<String> {
    if let Some(id) = &config.zone.id {
        return Ok(id.clone());
    } else {
        log::warn!("Zone ID is unset in configure");
        log::info!("start to get Zone ID");
    }
    let zones = client
        .get("https://api.cloudflare.com/client/v4/zones")
        .set_header(config)
        .send()
        .await?
        .json::<ApiResponse<Zones>>()
        .await?
        .solve()?;

    for zone in zones.zones {
        if zone.name == config.zone.name {
            return Ok(zone.id);
        }
    }

    Err(format!(
        "non of zones match the given zone name `{}`",
        config.zone.name
    ))?
}

#[derive(Debug, serde::Deserialize)]
pub struct Record {
    pub name: String,
    pub id: String,
    pub content: IpAddr,
}

#[derive(Debug)]
pub struct Records {
    records: Vec<Record>,
}

impl std::fmt::Display for Records {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Debug::fmt(self, f)
    }
}

impl<'de> serde::Deserialize<'de> for Records {
    fn deserialize<D>(deserializer: D) -> std::result::Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        Vec::<Record>::deserialize(deserializer).map(|zones| Records { records: zones })
    }
}

pub async fn get_dns_record(client: &reqwest::Client, zone_id: &str, name: &str) -> Result<Record> {
    let url = format!("https://api.cloudflare.com/client/v4/zones/{zone_id}/dns_records",);
    let records = client
        .get(url)
        .send()
        .await?
        .json::<ApiResponse<Records>>()
        .await?
        .solve()?;

    for record in records.records {
        if record.name == name && record.id == name {
            return Ok(record);
        }
    }

    Err(format!("DNS record `{name}` not found"))?
}

#[derive(Debug, serde::Deserialize)]
pub struct Overwrite {
    pub content: IpAddr,
    pub name: String,
}

async fn overwrite_dns_record_inner(
    type_: &str,
    client: &reqwest::Client,
    zone_id: &str,
    record_id: &str,
    ip_address: IpAddr,
    name: &str,
) -> Result<IpAddr> {
    let url = format!(
        "https://api.cloudflare.com/client/v4/zones/{zone_id}/dns_records/{record_id}",
        zone_id = zone_id,
        record_id = record_id
    );
    let body = serde_json::json!({
        "type": type_,
        "name": name,
        "content": ip_address.to_string(),
        "ttl": 1,
        "proxied": false,
    });
    let result = client
        .put(url)
        .json(&body)
        .send()
        .await?
        .json::<ApiResponse<Overwrite>>()
        .await?
        .solve()?;

    if result.name == name {
        Ok(result.content)
    } else {
        Err(format!(
            "Overwrite DNS record `{name}` error: expect name `{name}`, but `{}` found",
            result.name
        ))?
    }
}

pub async fn overwrite_ipv4_dns_record(
    client: &reqwest::Client,
    zone_id: &str,
    record_id: &str,
    ip_address: IpAddr,
    name: &str,
) -> Result<IpAddr> {
    overwrite_dns_record_inner("A", client, zone_id, record_id, ip_address, name).await
}

pub async fn overwrite_ipv6_dns_record(
    client: &reqwest::Client,
    zone_id: &str,
    record_id: &str,
    ip_address: IpAddr,
    name: &str,
) -> Result<IpAddr> {
    overwrite_dns_record_inner("AAAAA", client, zone_id, record_id, ip_address, name).await
}

pub async fn put_dns_record(
    client: &reqwest::Client,
    zone_id: &str,
    record_id: &str,
    ip_address: IpAddr,
    name: &str,
) -> Result<IpAddr> {
    match ip_address.get_ip_version() {
        crate::IpVersion::V4 => {
            overwrite_ipv4_dns_record(client, zone_id, record_id, ip_address, name).await
        }
        crate::IpVersion::V6 => {
            overwrite_ipv6_dns_record(client, zone_id, record_id, ip_address, name).await
        }
    }
}
