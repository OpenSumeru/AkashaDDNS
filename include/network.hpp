#pragma once
#include "pch.h"

inline bool isIPv4Valid(std::string& ip)
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

inline json getIPv4()
{
    static httplib::Client client("https://api.iplocation.net");
    client.set_address_family(AF_INET);
    auto getLocalIP = safeGet(client,"/?cmd=get-ip");
    if (!getLocalIP.has_value())
    {
        return json{false,false};
    }

    auto result = json::parse(getLocalIP.value()->body);

    //return result;

    auto rax = result["ip"].get<std::string>();
    if (!isIPv4Valid(rax))
    {
        return json{false,false};
    }
    auto getIPinfo = safeGet(client,"/?ip="+rax);
    if (!getIPinfo.has_value())
    {
        std::cout << "Fail to get IP Information" << std::endl;
        return result;
    }
    
    auto resultAdvance = json::parse(getIPinfo.value()->body);
    
    return resultAdvance;
}