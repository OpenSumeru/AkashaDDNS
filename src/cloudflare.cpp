#include "cloudflare.hpp"

Json getResult(httplib::Client& client, std::string path, std::string pathHead)
{
    static std::string pathStatic = "/client/v4";
    if (pathHead.length() > 0)
    {
        pathStatic = pathHead;
    }
    auto response = safeGet(client, pathStatic + path);
    if (!response.has_value())
    {
        return Json::array{false};
    }
    std::string error;
    Json result = Json::parse(response.value()->body, error);
    if (error.length() > 0)
    {
        std::cout << "Error: " << error << std::endl;
    }
    for (auto rax : result["messages"].array_items())
    {
        std::cout << "Code: " << rax["code"].int_value() << "; Message: " << rax["message"].string_value() << std::endl;
    }
    if (result["success"].bool_value())
    {
        return result["result"];
    }
    for (auto rax : result["errors"].array_items())
    {
        std::cout << "Error: " << rax.dump() << std::endl;
    }
    return Json::array{false};
}

Json putResult(httplib::Client& client, std::string path, std::string text, std::string type, std::string pathHead)
{
    static std::string pathStatic = "/client/v4";
    if (pathHead.length() > 0)
    {
        pathStatic = pathHead;
    }
    auto response = safePut(client, pathStatic + path, text, type);
    if (!response.has_value())
    {
        return Json::array{false};
    }
    std::string error;
    Json result = Json::parse(response.value()->body, error);
    if (error.length() > 0)
    {
        std::cout << "Error: " << error << std::endl;
    }
    for (auto rax : result["messages"].array_items())
    {
        std::cout << "Code: " << rax["code"].int_value() << "; Message: " << rax["message"].string_value() << std::endl;
    }
    if (result["success"].bool_value())
    {
        return result["result"];
    }
    for (auto rax : result["errors"].array_items())
    {
        std::cout << "Error: " << rax.dump() << std::endl;
    }
    return Json::array{false};
}

bool verifyAPI(httplib::Client& client, Json config)
{
    httplib::Headers header = {
        {"X-Auth-Email",config["Email"].string_value()},
        {config["API-Key-Type"].string_value()=="Auth"?"X-Auth-Key":"Authorization",
        config["API-Key-Type"].string_value()=="Auth"?config["API-Key"].string_value():"Bearer "+config["API-Key"].string_value()}
    };
    client.set_default_headers(header);
    return getResult(client,"/user/tokens/verify")[0].bool_value();
}

std::string findZoneId(httplib::Client& client, Json config)
{
    if (config["ID"].string_value().length() > 0)
    {
        return config["ID"].string_value();
    }
    for (auto rax : getResult(client, "/zones").array_items())
    {
        if (rax["name"].string_value() == config["Name"].string_value())
        {
            return config["ID"].string_value();
        }
        else if (rax.is_bool() && !rax.bool_value())
        {
            break;
        }
    }
    std::cout << "Zone " << config["Name"].string_value() << "not found" << std::endl;
    return "";
}

std::string findRecordId(httplib::Client& client, std::string zoneId, std::string name)
{
    auto json = getResult(client, "/zones/" + zoneId + "/dns_records");
    for (auto rax : json.array_items())
    {
        if (rax["name"].string_value() == name)
        {
            return rax["id"].string_value();
        }
        else if (rax.is_bool() && !rax.bool_value())
        {
            break;
        }
    }
    std::cout << "DNS Record " << name << "not found" << std::endl;
    return "";
}

std::string putRecordId(httplib::Client& client, std::string zoneId, std::string dnsRecordId, std::string ipAddress, std::string name)
{
    Json requestContent = Json::object {{"content", ipAddress},{"name", name},{"type", "A"}};
    auto json = putResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId, requestContent.dump(), "application/json");
    if (json["name"].string_value() == name)
    {
        return json["content"].string_value();
    }
    std::cout << "Put DNS Record Error" << std::endl;
    return "";
}