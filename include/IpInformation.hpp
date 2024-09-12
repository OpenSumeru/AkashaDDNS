#ifndef IP_INFORMATION
#define IP_INFORMATION
#include <optional>
#include <string>

#include "pch.hpp"

struct IpInfo
{
    std::optional<std::string> country;
    std::optional<std::string> countryRegion;
    std::optional<std::string> region;
    std::optional<std::string> city;
    std::optional<std::string> asOrganization;
    std::optional<double> latitude;
    std::optional<double> longitude;
    virtual void setIpInfo(const json &j);
    virtual void printIpInfo();
};
#endif