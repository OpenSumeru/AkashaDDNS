#include "network.hpp"

bool isIPv4Valid(std::string& ip)
{
    static const std::regex IPv4Pattern("\\d+\\.\\d+\\.\\d+\\.\\d+");
    if (std::regex_match(ip,IPv4Pattern))
    {
        return true;
    }
    else
    {
        std::cout << "Error IPv4 Pattern: IP=" << ip << std::endl;
        return false;
    }
}

bool isIPv6Valid(std::string& ip)
{
    static const std::regex IPv6Pattern("[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+");
    if (std::regex_match(ip,IPv6Pattern))
    {
        return true;
    }
    else
    {
        std::cout << "Error IPv6 Pattern: IP=" << ip << std::endl;
        return false;
    }
}

json getIPv4()
{
    static httplib::Client client("https://api.seeip.org");
    client.set_address_family(AF_INET);
    auto getLocalIP = safeGet(client,"/geoip");
    if (!getLocalIP.has_value())
    {
        return json{false,false};
    }

    auto result = json::parse(getLocalIP.value()->body);

    return result;
}

json getIPv6()
{
    static httplib::Client client("https://api.seeip.org");
    client.set_address_family(AF_INET6);
    auto getLocalIP = safeGet(client,"/geoip");
    if (!getLocalIP.has_value())
    {
        return json{false,false};
    }

    auto result = json::parse(getLocalIP.value()->body);

    return result;
}