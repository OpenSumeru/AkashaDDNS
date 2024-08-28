#include "cloudflare.hpp"

json getResult(httplib::Client &client, std::string path, std::string pathHead)
{
    static std::string pathStatic = "/client/v4";
    if (pathHead.length() > 0)
    {
        pathStatic = pathHead;
    }
    auto response = safeGet(client, pathStatic + path);
    if (!response.has_value())
    {
        return json{false, false};
    }
    json result = json::parse(response.value()->body);
    for (auto rax : result["messages"])
    {
        std::cout << "Code: " << rax["code"].get<size_t>() << "; Message: " << rax["message"].get<std::string>()
                  << std::endl;
    }
    if (result["success"].get<bool>())
    {
        return result["result"];
    }
    for (auto rax : result["errors"])
    {
        std::cout << "\033[0;31mError: \033[0m" << rax.dump() << std::endl;
    }
    return json{false, false};
}

json putResult(httplib::Client &client, std::string path, std::string text, std::string type, std::string pathHead)
{
    static std::string pathStatic = "/client/v4";
    if (pathHead.length() > 0)
    {
        pathStatic = pathHead;
    }
    auto response = safePut(client, pathStatic + path, text, type);
    if (!response.has_value())
    {
        return json{false, false};
    }
    json result = json::parse(response.value()->body);
    for (auto rax : result["messages"])
    {
        std::cout << "Code: " << rax["code"].get<size_t>() << "; Message: " << rax["message"].get<std::string>()
                  << std::endl;
    }
    if (result["success"].get<bool>())
    {
        return result["result"];
    }
    for (auto rax : result["errors"])
    {
        std::cout << "\033[0;31mError: \033[0m" << rax.dump() << std::endl;
    }
    return json{false, false};
}

bool verifyAPI(httplib::Client &client, json config)
{
    httplib::Headers header = {{"X-Auth-Email", config["Email"].get<std::string>()},
                               {config["API-Key-Type"].get<std::string>() == "Auth" ? "X-Auth-Key" : "Authorization",
                                config["API-Key-Type"].get<std::string>() == "Auth"
                                    ? config["API-Key"].get<std::string>()
                                    : "Bearer " + config["API-Key"].get<std::string>()}};
    client.set_default_headers(header);
    return getResult(client, "/user/tokens/verify").is_array();
}

std::string findZoneId(httplib::Client &client, json config)
{
    std::string eax;
    try
    {
        config.at("ID").get_to(eax);
    }
    catch (std::exception &error)
    {
        std::cout << "No Zone ID in Configure\nStart to Get Zone ID from Cloudflare" << std::endl;
    }
    if (eax.length() > 0)
    {
        return eax;
    }
    for (auto rax : getResult(client, "/zones"))
    {
        if (rax["name"].get<std::string>() == config["Name"].get<std::string>())
        {
            return rax["id"].get<std::string>();
        }
        else if (rax.is_array())
        {
            break;
        }
    }
    std::cout << "Zone " << config["Name"].get<std::string>() << " not found" << std::endl;
    return "";
}

std::string findRecordId(httplib::Client &client, std::string zoneId, std::string name)
{
    auto json = getResult(client, "/zones/" + zoneId + "/dns_records");
    for (auto rax : json)
    {
        if (rax["name"].get<std::string>() == name)
        {
            return rax["id"].get<std::string>();
        }
        else if (rax.is_array())
        {
            break;
        }
    }
    std::cout << "\033[0;31mDNS Record " << name << " not found\033[0m" << std::endl;
    return "";
}

std::string putIPv4RecordId(httplib::Client &client, std::string zoneId, std::string dnsRecordId, std::string ipAddress,
                            std::string name)
{
    json requestContent = json{{"content", ipAddress}, {"name", name}, {"type", "A"}};
    auto json = putResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId, requestContent.dump(),
                          "application/json");
    try
    {
        if (json["name"].get<std::string>() == name)
        {
            return json["content"].get<std::string>();
        }
    }
    catch (std::exception &error)
    {
        std::cout << "\033[0;31mPut DNS Record Error\033[0m" << std::endl;
    }
    return "";
}

std::string putIPv6RecordId(httplib::Client &client, std::string zoneId, std::string dnsRecordId, std::string ipAddress,
                            std::string name)
{
    json requestContent = json{{"content", ipAddress}, {"name", name}, {"type", "AAAAA"}};
    auto json = putResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId, requestContent.dump(),
                          "application/json");
    try
    {
        if (json["name"].get<std::string>() == name)
        {
            return json["content"].get<std::string>();
        }
    }
    catch (std::exception &error)
    {
        std::cout << "\033[0;31mPut DNS Record Error\033[0m" << std::endl;
    }
    return "";
}