use reqwest::{header::HeaderMap, Client, ClientBuilder};
use serde_json::Value;

const PATH_STATIC: &str = "https://api.cloudflare.com/client/v4";

pub async fn get_result(
    client: &Client,
    path: &str,
    // path_head: Option<&str>,
) -> crate::Result<Value> {
    let mut result = client
        .get(format!("{}{}", PATH_STATIC, path))
        .send()
        .await?
        .json::<Value>()
        .await?;
    for rax in result["messages"].as_array().unwrap() {
        println!(
            "Code: {}; Message: {}",
            rax["code"].as_number().unwrap(),
            rax["message"].as_str().unwrap()
        );
    }
    if result["success"].as_bool().is_some_and(|b| b) {
        return Ok(result["result"].take());
    }
    for rax in result["errors"].as_array().unwrap() {
        eprintln!("Error: {}", rax)
    }
    panic!()
}

pub async fn put_result(client: &Client, path: &str, text: String) -> crate::Result<Value> {
    let mut result = client
        .put(format!("{}{}", PATH_STATIC, path))
        .body(text)
        .send()
        .await?
        .json::<Value>()
        .await?;
    for rax in result["messages"].as_array().unwrap() {
        println!(
            "Code: {}; Message: {}",
            rax["code"].as_str().unwrap(),
            rax["message"].as_str().unwrap()
        );
    }
    if result["success"].as_bool().is_some_and(|b| b) {
        return Ok(result["result"].take());
    }
    for rax in result["errors"].as_array().unwrap() {
        eprintln!("Error: {}", rax)
    }
    panic!()
}

pub async fn verify_api(config: &Value) -> crate::Result<Client> {
    /*
    {"X-Auth-Email",config["Email"].string_value()},
    {config["API-Key-Type"].string_value()=="Auth"?"X-Auth-Key":"Authorization",
    config["API-Key-Type"].string_value()=="Auth"?config["API-Key"].string_value():"Bearer "+config["API-Key"].string_value()}
     */
    let mut header = HeaderMap::new();
    header.insert("X-Auth-Email", config["Email"].as_str().unwrap().parse()?);
    if config["API-Key-Type"] == "Auth" {
        header.insert("X-Auth-Key", config["API-Key"].as_str().unwrap().parse()?);
    } else {
        header.insert(
            "Authorization",
            format!("Bearer {}", config["API-Key"].as_str().unwrap()).parse()?,
        );
    };

    let client = ClientBuilder::new().default_headers(header).build()?;
    get_result(&client, "/user/tokens/verify").await?;
    Ok(client)
}

pub async fn find_zone_id(client: &Client, config: &Value) -> crate::Result<String> {
    let id = config["ID"]
        .as_str()
        .map(|s| s.to_owned())
        .unwrap_or_default();

    if !id.is_empty() {
        return Ok(id);
    }

    let json = get_result(client, "/zones").await?;
    let json = json.as_array().unwrap();

    let rax = json
        .iter()
        .find(|rax| rax["name"] == config["Name"])
        .unwrap_or_else(|| {
            panic!(
                "Find Zone ID Error: Zone {} not found",
                config["Name"].as_str().unwrap()
            )
        });

    Ok(rax["id"].as_str().unwrap().to_owned())
}

pub async fn find_record_id(client: &Client, zond_id: &str, name: &str) -> crate::Result<String> {
    let json = get_result(client, &format!("/zones/{}/dns_records", zond_id)).await?;
    let json = json.as_array().unwrap();

    let rax = json
        .iter()
        .find(|rax| rax["name"] == name)
        .unwrap_or_else(|| panic!("DNS Record ID Error: DNS Record {} not found", name));
    Ok(rax["id"].as_str().unwrap().to_owned())
}

pub async fn put_record_id(
    client: &Client,
    zond_id: &str,
    dns_record_id: &str,
    ip_address: &str,
    name: &str,
) -> crate::Result<String> {
    let content = serde_json::json! ({
        "content": ip_address,
        "name": name,
        "type": "A",
    });

    let path = format!("/zones/{}/dns_records/{}", zond_id, dns_record_id);
    let json = put_result(client, &path, content.to_string()).await?;

    if json["name"] == name {
        return Ok(json["content"].as_str().unwrap().to_owned());
    }
    eprintln!("Put DNS Record Error");
    Ok(String::new())
}
