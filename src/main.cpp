#include "pch.h"
#include "network.hpp"
#include "cloudflare.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few parameters" << std::endl;
        return -1;
    }

    std::string configPath = argv[1];
    httplib::Client client("https://api.cloudflare.com");
    if (configPath.length() == 0)
    {
        configPath = "config.json";
    }
    auto config = readFileTojson(configPath);

    if (verifyAPI(client, config["Headers"]))
    {
        std::cout << "Verify API Key Error" << std::endl;
        return 0;
    }

    auto&& zoneId = findZoneId(client, config["Zone"]);
    if (zoneId.length() == 0)
    {
        std::cout << "Find Zone ID Error" << std::endl;
        return 0;
    }
    auto dnsName = config["Target"]["DNS-Name"].get<std::string>();
    std::cout << "Found Zone ID" << std::endl;

    auto&& dnsRecordId = findRecordId(client, zoneId, dnsName);
    if (dnsRecordId.length() == 0)
    {
        std::cout << "DNS Record ID Error" << std::endl;
        return 0;
    }
    std::cout << "DNS Record ID OK" << std::endl;

    std::string ipAddress = getResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId)["content"].get<std::string>();
    std::cout << "Start Looping" << std::endl;
    while (true)
    {
        auto rax = getIPv4();
        if (rax.is_array())
        {
            std::cout << "Fail to Get Local IP" << std::endl;
            continue;
        }
        auto rbx = rax["ip"].get<std::string>();
        rbx = isIPv4Valid(rbx)? rbx : ipAddress;

        if (rbx != ipAddress)
        {
            std::cout << "Changing from " << ipAddress << " to " << rbx << std::endl;
            auto&& reciveIp = putRecordId(client, zoneId, dnsRecordId, rbx, dnsName);
            if (reciveIp == rbx)
            {
                if (rax["country_code2"].is_string() && rax["isp"].is_string())
                {
                    ipAddress = rbx;
                    std::cout << "Local IP: " << ipAddress << "; From: " << rax["country_code2"].get<std::string>() << "; Belong: " << rax["isp"].get<std::string>() << std::endl;
                }
                else
                {
                    ipAddress = rbx;
                    std::cout << "Local IP: " << ipAddress << std::endl;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
    return 0;
}
