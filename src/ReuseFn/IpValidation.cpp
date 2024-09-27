#include <fmt/printf.h>
#include <regex>

#include "ReuseFn/IpValidation.hpp"

bool isValidIpV4(std::string IpAddress)
{
    static const std::regex IPv4Pattern("\\d+\\.\\d+\\.\\d+\\.\\d+");
    if (std::regex_match(IpAddress, IPv4Pattern))
    {
        return true;
    }
    else
    {
        fmt::println("\033[0;31mError IPv4 Pattern: IP={}\033[0m", IpAddress);
        return false;
    }
}

bool isValidIpV6(std::string IpAddress)
{
    static const std::regex IPv6Pattern(
        "[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+:[0-9a-z]+");
    if (std::regex_match(IpAddress, IPv6Pattern))
    {
        return true;
    }
    else
    {
        fmt::println("\033[0;31mError IPv6 Pattern: IP={}\033[0m", IpAddress);
        return false;
    }
}