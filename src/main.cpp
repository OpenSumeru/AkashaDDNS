#include "pch.h"
#include "network.hpp"
#include "cloudflare.hpp"

int main(size_t argc, char** argv)
{
    std::string configPath = argv[1];
    auto client = std::make_shared<httplib::Client>("https://api.cloudflare.com");
    if (configPath.length() == 0)
    {
        configPath = "config.json";
    }
    auto config = readFileToJson(configPath);
    verifyAPI(*client, config["Headers"]);
    auto zoneId = findZoneId(*client, config["Zone"]);
    if (zoneId.length() == 0)
    {
        std::cout << "Find Zone ID Error" << std::endl;
        return 0;
    }
    auto dnsName = config["Target"]["DNS-Name"].string_value();
    auto dnsRecordId = findRecordId(*client, zoneId, dnsName);
    if (dnsRecordId.length() == 0)
    {
        std::cout << "DNS Record ID Error" << std::endl;
        return 0;
    }
    auto ipAddress = getResult(*client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId)["content"].string_value();
    while (true)
    {
        auto rax = getIPv4();
        if (rax != ipAddress)
        {
            auto reciveIp = putRecordId(*client, zoneId, dnsRecordId, rax, dnsName);
            if (reciveIp == rax)
            {
                ipAddress = rax;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
    return 0;
}
