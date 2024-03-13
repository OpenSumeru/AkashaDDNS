#include "pch.h"
#include "network.hpp"
#include "cloudflare.hpp"

void quitProgram()
{
    while (getchar() != 'q')
    {

    }
    exit(0);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few parameters" << std::endl;
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
        std::cout << "Fail to Read IP from Cloudflare" << std::endl;
        }

        std::string rbx;
        rax = getIP();
        try
        {
        if (rax.is_array())
        {
            std::cout << "Fail to Get Local IP" << std::endl;
            continue;
        }
        rbx = rax["ip"].get<std::string>();
        }
        catch (std::exception& error)
        {
        std::cout << "Fail to Read IP" << std::endl;
        }
        rbx = isIPValid(rbx)? rbx : ipAddress;

        if (rbx != ipAddress)
        {

            std::cout << "Changing from " << ipAddress << " to " << rbx << std::endl;
            
            auto&& reciveIp = putRecordId(client, zoneId, dnsRecordId, rbx, dnsName);
            if (reciveIp == rbx)
            {
                try
                {
                if (rax["country_code"].is_string() && rax["organization"].is_string())
                {
                    std::cout << "Local IP:" << ipAddress 
                    << " From:" << rax["country_code"].get<std::string>() 
                    << " Belong:" << rax["organization"].get<std::string>()
                    << " Longitude:" << rax["longitude"].get<std::string>() 
                    << " Latitude:" << rax["latitude"].get<std::string>() << std::endl;
                }
                }
                catch (std::exception& error)
                {   
                    std::cout << "Fail to Print Advance Information" << std::endl;
                    std::cout << "Local IP: " << ipAddress << std::endl;
                }
                
            }
            
        }
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
    
    quitRunner.join();
    return 0;
}
