#ifndef IP_VALIDATION_HPP
#define IP_VALIDATION_HPP

#include <string>

bool isValidIpV4(const std::string &IpAddress);
bool isValidIpV6(const std::string &IpAddress);

enum class IpVersion
{
    V4 = 4,
    V6 = 6
};

#endif