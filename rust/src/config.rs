#[derive(serde::Deserialize)]
pub struct Config {
    #[serde(rename = "Header")]
    pub header: Header,
    #[serde(rename = "Zone")]
    pub zone: Zone,
    #[serde(rename = "Target")]
    pub target: Target,
    #[serde(default)]
    #[serde(rename = "Setting")]
    pub setting: Setting,
}

#[derive(serde::Deserialize)]
pub struct Header {
    #[serde(rename = "API-Key-Type")]
    pub api_key_type: String,
    #[serde(rename = "API-Key")]
    pub api_key: String,
    #[serde(rename = "Email")]
    pub email: String,
}

#[derive(serde::Deserialize)]
pub struct Zone {
    #[serde(rename = "Name")]
    pub name: String,
    #[serde(default)]
    pub id: Option<String>,
}

#[derive(serde::Deserialize)]
pub struct Target {
    #[serde(rename = "DNS-Name")]
    pub dns_name: String,
}

#[derive(Default, serde::Deserialize)]
pub struct Setting {
    #[serde(rename = "IP-Version")]
    pub ip_version: IpVersion,
}

#[derive(Default, serde::Deserialize)]
pub enum IpVersion {
    IPv4,
    IPv6,
    #[serde(skip)]
    #[default]
    Auto,
}
