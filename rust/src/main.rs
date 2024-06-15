use cloudflare::*;
use config::Config;
use network::*;
use std::{process::ExitCode, time::Duration};
use tokio::fs;

mod cloudflare;
mod config;
mod network;

type Result<T = (), E = Box<dyn std::error::Error>> = std::result::Result<T, E>;

#[tokio::main]
async fn main() -> Result<ExitCode> {
    env_logger::builder()
        .filter_level(log::LevelFilter::Info)
        .init();

    let main = tokio::spawn(async move {
        let Err(err) = run().await else {
            unreachable!()
        };
        log::error!("{err}");
    });

    tokio::select! {
        _ = main => {
            log::info!("stoped unexpectly by error");
            Ok(ExitCode::FAILURE)
        }
        _ = tokio::signal::ctrl_c() => {
            log::info!("stoped by ctrl-c");
            Ok(ExitCode::SUCCESS)
        }
    }
}

async fn run() -> Result<()> {
    let config_path = std::env::args().nth(1).unwrap_or_else(|| {
        log::warn!("too few arguments!");
        log::warn!("config_path set to ./config.json as default");
        "config.json".to_owned()
    });

    let config: Config = serde_json::from_str(&fs::read_to_string(config_path).await?)?;

    let (client, ip) = match config.setting.ip_version {
        config::IpVersion::IPv4 => init_ipv4().await?,
        config::IpVersion::IPv6 => init_ipv6().await?,
        config::IpVersion::Auto => init_unordered().await?,
    };
    log::info!("local ip: {ip}");
    verify_api(&client, &config).await?;
    log::info!("api key verified");
    let zone_id = find_zone_id(&client, &config).await?;
    log::info!("found zone id");

    let mut ip;
    let dns_name = &config.target.dns_name;
    log::info!("start looping");

    loop {
        let record = get_dns_record(&client, &zone_id, dns_name).await?;
        ip = record.content;
        let real_ip = get_ip(&client, ip.get_ip_version()).await?;
        if real_ip != ip {
            log::info!("changeing from `{ip}` to {real_ip}");
            put_dns_record(&client, &zone_id, &record.id, real_ip, dns_name).await?;
        }
        tokio::time::sleep(Duration::from_secs(60)).await;
    }
}
