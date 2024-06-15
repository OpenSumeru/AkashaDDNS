use crate::Result;
use std::{
    fmt::Display,
    future::Future,
    net::{IpAddr, Ipv4Addr, Ipv6Addr},
};

#[derive(Debug, Clone, Copy)]
pub enum IpVersion {
    V4,
    V6,
}

impl std::fmt::Display for IpVersion {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            IpVersion::V4 => "ipv4".fmt(f),
            IpVersion::V6 => "ipv6".fmt(f),
        }
    }
}
pub trait GetIpVersion {
    fn get_ip_version(&self) -> IpVersion;
}

impl GetIpVersion for IpAddr {
    fn get_ip_version(&self) -> IpVersion {
        match self {
            IpAddr::V4(ip) => ip.get_ip_version(),
            IpAddr::V6(ip) => ip.get_ip_version(),
        }
    }
}

impl GetIpVersion for Ipv4Addr {
    fn get_ip_version(&self) -> IpVersion {
        IpVersion::V4
    }
}

impl GetIpVersion for Ipv6Addr {
    fn get_ip_version(&self) -> IpVersion {
        IpVersion::V6
    }
}

impl GetIpVersion for IpVersion {
    fn get_ip_version(&self) -> IpVersion {
        *self
    }
}

pub async fn init_unordered() -> Result<(reqwest::Client, IpAddr)> {
    let (sender1, mut receiver) = tokio::sync::mpsc::channel(2);
    let sender2 = sender1.clone();

    let t1 = tokio::spawn(async move {
        let Ok(ca) = init_ipv4().await else {
            return;
        };
        _ = sender1.send(ca).await;
    });
    let t2 = tokio::spawn(async move {
        let Ok(ca) = init_ipv6().await else {
            return;
        };
        _ = sender2.send(ca).await;
    });

    match receiver.recv().await {
        Some(ca) => {
            t1.abort();
            t2.abort();
            Ok(ca)
        }
        None => Err("cant get neither ipv4 addr nor ipv6 addr")?,
    }
}

pub async fn init_ipv4() -> Result<(reqwest::Client, IpAddr)> {
    init_inner(IpVersion::V4).await
}

pub async fn init_ipv6() -> Result<(reqwest::Client, IpAddr)> {
    init_inner(IpVersion::V6).await
}

pub async fn init_inner(ip_version: IpVersion) -> Result<(reqwest::Client, IpAddr)> {
    let local_addr = match ip_version {
        IpVersion::V4 => "127.0.0.1",
        IpVersion::V6 => "[0::]",
    };

    let client = reqwest::Client::builder()
        .local_address(local_addr.parse().ok())
        .build()?;
    let addr = get_ip(&client, ip_version).await?;
    Ok((client, addr))
}

pub async fn get_ip(client: &reqwest::Client, ip_version: IpVersion) -> Result<IpAddr> {
    let api = match ip_version {
        IpVersion::V4 => "https://api.ipify.org",
        IpVersion::V6 => "https://api.seeip.org",
    };
    r#try(&format!("get {ip_version} addr"), || async {
        let response = client.get(api).send().await?;
        let text = response.text().await?;
        let ip_addr = match ip_version {
            IpVersion::V4 => text.parse::<Ipv4Addr>().map(IpAddr::from),
            IpVersion::V6 => text.parse::<Ipv6Addr>().map(IpAddr::from),
        };
        Ok(ip_addr.map_err(|e| format!("parsing {text}: {e}"))?)
    })
    .await
}

pub async fn r#try<O, F>(desc: &str, task: impl Fn() -> F) -> Result<O>
where
    O: Display,
    F: Future<Output = Result<O>> + Send,
{
    const TRY_TIMES: usize = 11; // 4514

    for n in 1..=TRY_TIMES {
        log::info!("trying to {desc} ({n}/{TRY_TIMES})...");
        match task().await {
            Ok(ok) => {
                log::trace!("success to {desc}: {ok}");
                return Ok(ok);
            }
            Err(error) => {
                log::error!("failed to {desc} ({n}/{TRY_TIMES}): {error}");
                if n == TRY_TIMES {
                    return Err(error);
                }
            }
        }
    }
    unreachable!()
}
