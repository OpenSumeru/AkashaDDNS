use reqwest::{header::HeaderMap, Client, ClientBuilder};
use serde_json::Value;

pub async fn get_result(
    client: &Client,
    path: &str,
    path_head: Option<&str>,
) -> crate::Result<Value> {
    let path_static = path_head.unwrap_or("/client/v4");
    let mut result = client
        .get(format!("{}{}", path_static, path))
        .send()
        .await?
        .json::<Value>()
        .await?;
    for rax in result["messages"].as_array().unwrap() {
        println!("Code: {}; Message: {}", rax["code"], rax["message"]);
    }
    if result["success"].as_bool().is_some_and(|b| b) {
        return Ok(result["result"].take());
    }
    for rax in result["errors"].as_array().unwrap() {
        eprintln!("Error: {}", rax)
    }
    Ok(Value::Bool(false))
}

pub async fn put_result(
    client: &Client,
    path: &str,
    text: String,
    // r#type: &str,
    path_head: Option<&str>,
) -> crate::Result<Value> {
    let path_static = path_head.unwrap_or("/client/v4");
    let mut result = client
        .put(format!("{}{}", path_static, path))
        .body(text)
        .send()
        .await?
        .json::<Value>()
        .await?;
    for rax in result["messages"].as_array().unwrap() {
        println!("Code: {}; Message: {}", rax["code"], rax["message"]);
    }
    if result["success"].as_bool().is_some_and(|b| b) {
        return Ok(result["result"].take());
    }
    for rax in result["errors"].as_array().unwrap() {
        eprintln!("Error: {}", rax)
    }
    Ok(Value::Bool(false))
}

pub async fn verify_api(config: &Value) -> crate::Result<Client> {
    /*
    {"X-Auth-Email",config["Email"].string_value()},
    {config["API-Key-Type"].string_value()=="Auth"?"X-Auth-Key":"Authorization",
    config["API-Key-Type"].string_value()=="Auth"?config["API-Key"].string_value():"Bearer "+config["API-Key"].string_value()}
     */
    let mut header = HeaderMap::new();
    header.insert("X-Auth-Email", config["Email"].to_string().parse()?);
    if config["API-Key-Type"] == "Auth" {
        header.insert("X-Auth-Key", config["API-Key"].to_string().parse()?);
    } else {
        header.insert(
            "Authorization",
            format!("Bearer {}", config["API-Key"]).parse()?,
        );
    };

    Ok(ClientBuilder::new().default_headers(header).build()?)
}

pub async fn find_zone_id(client: &Client, config: &Value) -> crate::Result<String> {
    if !config["ID"].to_string().is_empty() {
        return Ok(config["ID"].to_string());
    }
    for rax in get_result(client, "/zones", None)
        .await?
        .as_array()
        .unwrap()
    {
        if rax["name"] == config["Name"] {
            return Ok(config["ID"].to_string());
        } else if rax.as_bool().is_some_and(|b| !b) {
            break;
        }
    }

    panic!("Find Zone ID Error: Zone {} not found", config["Name"]);
}

pub async fn find_record_id(client: &Client, zond_id: &str, name: &str) -> crate::Result<String> {
    let json = get_result(client, &format!("/zones/{}/dns_records", zond_id), None).await?;

    for rax in json.as_array().unwrap() {
        if rax["name"] == name {
            return Ok(rax["id"].to_string());
        } else if rax.as_bool().is_some_and(|b| !b) {
            break;
        }
    }

    panic!("DNS Record ID Error: DNS Record {} not found", name);
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
    let json = put_result(
        client,
        &format!("/zones/{}/dns_records/{}", zond_id, dns_record_id),
        content.to_string(),
        None,
    )
    .await?;
    if json["name"] == name {
        return Ok(json["content"].to_string());
    }
    eprintln!("Put DNS Record Error");
    Ok(String::new())
}
