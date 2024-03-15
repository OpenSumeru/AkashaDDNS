#include "pch.h"
#include "network.hpp"
#include "cloudflare.hpp"

void quitProgram()
{
    if (getchar() == 'q')
    {
        exit(0);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "\033[0;31mToo few parameters\033[0m" << std::endl;
        return -1;
    }

    std::function<bool(std::string&)> isIPValid = isIPv4Valid;
    std::function<json()> getIP = getIPv4;
    std::function<std::string(httplib::Client& client, std::string zoneId, std::string dnsRecordId, std::string ipAddress, std::string name)> 
    putRecordId = putIPv4RecordId;

    std::string configPath = argv[1];
    httplib::Client client("https://api.cloudflare.com");
    if (configPath.length() == 0)
    {
        configPath = "config.json";
    }
    auto config = readFileTojson(configPath);

    std::string versionIP;
    try
    {
        config.at("Setting").at("IP-Version").get_to(versionIP);
    }
    catch (std::exception& error)
    {
        std::cout << "No Setting Has Found\nStart Program with Default Settings" << std::endl;
    }
    if (versionIP == "IPv6")
    {
        isIPValid = isIPv6Valid;
        getIP = getIPv6;
        putRecordId = putIPv6RecordId;
        client.set_address_family(AF_INET6);
    }
    else
    {
        client.set_address_family(AF_INET);
    }
    client.set_keep_alive(true);

    if (verifyAPI(client, config["Header"]))
    {
        std::cout << "\033[0;31mVerify API Key Error\033[0m" << std::endl;
        return 0;
    }

    auto&& zoneId = findZoneId(client, config["Zone"]);
    if (zoneId.length() == 0)
    {
        std::cout << "\033[0;31mFind Zone ID Error\033[0m" << std::endl;
        return 0;
    }
    std::cout << "Found Zone ID" << std::endl;
    
    auto dnsName = config["Target"]["DNS-Name"].get<std::string>();
    auto&& dnsRecordId = findRecordId(client, zoneId, dnsName);
    if (dnsRecordId.length() == 0)
    {
        std::cout << "\033[0;31mDNS Record ID Error\033[0m" << std::endl;
        return 0;
    }
    std::cout << "DNS Record ID OK" << std::endl;
    std::string ipAddress = getResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId)["content"].get<std::string>();
    std::thread quitRunner(quitProgram);
    std::cout << "Start Looping" << std::endl;
    while (true)
    {
        json rax = getResult(client, "/zones/" + zoneId + "/dns_records/" + dnsRecordId);
        try
        {
            rax.at("content").get_to(ipAddress);
        }
        catch (std::exception& error)
        {
        std::cout << "\033[0;31mFail to Read IP from Cloudflare\033[0m" << std::endl;
        }

        std::string rbx;
        rax = getIP();
        try
        {
        if (rax.is_array())
        {
            std::cout << "\033[0;31mFail to Get Local IP\033[0m" << std::endl;
            continue;
        }
        rbx = rax["ip"].get<std::string>();
        }
        catch (std::exception& error)
        {
        std::cout << "\033[0;31mFail to Read IP\033[0m" << std::endl;
        }
        rbx = isIPValid(rbx)? rbx : ipAddress;

        if (rbx != ipAddress)
        {

            std::cout << "Changing from " << ipAddress << " to " << rbx << std::endl;
            
            auto&& reciveIp = putRecordId(client, zoneId, dnsRecordId, rbx, dnsName);
            if (reciveIp == rbx)
            {
                ipAddress = rbx;
                try
                {
                if (rax["country_code"].is_string() && rax["organization"].is_string())
                {
                    std::cout << "\033[0;32mLocal IP: \033[0m" << ipAddress 
                    << "\033[0;32m From: \033[0m" << rax["country_code"].get<std::string>() 
                    << "\033[0;32m Belong: \033[0m" << rax["organization"].get<std::string>()
                    << "\033[0;32m Longitude: \033[0m" << rax["longitude"].get<std::string>() 
                    << "\033[0;32m Latitude: \033[0m" << rax["latitude"].get<std::string>() << std::endl;
                }
                }
                catch (std::exception& error)
                {   
                    std::cout << "\033[0;31mFail to Print Advance Information\033[0m" << std::endl;
                    std::cout << "\033[0;32mLocal IP: \033[0m" << ipAddress << std::endl;
                }
                
            }
            
        }
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
    
    quitRunner.join();
    return 0;
}
