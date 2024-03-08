#![allow(non_snake_case)]

mod cloudflare;

type Result<T = ()> = std::result::Result<T, Box<dyn std::error::Error>>;

#[tokio::main]
async fn main() -> Result {
    let Some(config_path) = std::env::args().nth(1) else {
        panic!("too few arguments")
    };

    let config = tokio::fs::read_to_string(&config_path).await?;
    let config: serde_json::Value = config.parse()?;

    let client = cloudflare::verify_api(config.get("Headers").unwrap()).await?;

    let zone_id = cloudflare::find_zone_id(&client, &config["Zone"]).await?;
    let dns_name = config["Target"]["DNS-Name"].as_str().unwrap();
    let dns_record_id = cloudflare::find_record_id(&client, &zone_id, dns_name).await?;

    let mut ip_address = cloudflare::get_result(
        &client,
        &format!("/zones/{}/dns_records/{}", zone_id, dns_record_id),
    )
    .await?["content"]
        .as_str()
        .unwrap()
        .to_owned();

    loop {
        let rax = get_ipv4().await?;
        if rax != ip_address {
            println!("Changing from {} to {}", ip_address, rax);
            let receive_ip =
                cloudflare::put_record_id(&client, &zone_id, &dns_record_id, &rax, dns_name)
                    .await?;
            if receive_ip == rax {
                ip_address = rax;
                println!("Local IP: {}", ip_address);
            }
        }

        tokio::time::sleep(std::time::Duration::from_secs(100)).await;
    }
}

#[inline]
async fn get_ipv4() -> Result<String> {
    Ok(reqwest::get("https://api.ipify.org").await?.text().await?)
}
