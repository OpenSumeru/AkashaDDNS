#include <fmt/printf.h>

#include "DdnsApiTrait.hpp"
#include "fmt/base.h"

void DDNS_API::printIpInfo(const std::string &ip)
{
    fmt::println("No ip information for {}", ip);
}

DDNS_API::DDNS_API(IpVersion version) : version_{version}
{
    switch (version_)
    {
    case IpVersion::V4:
        isValidIp = isValidIpV4;
        break;
    case IpVersion::V6:
        isValidIp = isValidIpV6;
        break;
    }
}

DDNS_API_Code DDNS_API::ddnsCycle()
{
    auto ip = getLocalIp();
    if (!isValidIp(ip))
    {
        return DDNS_API_Code::INVALID_IP;
    }
    auto ipRecord = getRecordIp();
    if (!isValidIp(ipRecord))
    {
        return DDNS_API_Code::INVALID_IP;
    }
    if (ipRecord == ip)
    {
        return DDNS_API_Code::NO_CHANGE;
    }
    fmt::println("Changing IP record from {} to {}", ipRecord, ip);
    printIpInfo(ip);
    if (setRecordIp(ip) != ip)
    {
        fmt::println("\033[0;31mSet IP record Error\033[0m");
        return DDNS_API_Code::SETTING_IP_ERROR;
    }
    if (getRecordIp() != ip)
    {
        fmt::println("\033[0;31mChange IP record Error\033[0m");
        return DDNS_API_Code::CHANGE_IP_ERROR;
    }
    fmt::println("\033[0;32mSuccessfully change local IP to {}\033[0m", ip);
    return DDNS_API_Code::IP_CHANGED;
}