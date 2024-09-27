#ifndef IP_INFORMATION_HPP
#define IP_INFORMATION_HPP
#include <optional>
#include <string>

#include "../pch.hpp"

struct IpInfo
{
    std::optional<std::string> country;
    std::optional<std::string> countryRegion;
    std::optional<std::string> region;
    std::optional<std::string> city;
    std::optional<std::string> asOrganization;
    std::optional<size_t> asn;
    std::optional<double> latitude;
    std::optional<double> longitude;
    virtual void setIpInfo(const json &j);
    virtual void printIpInfo(std::ostream &out = std::cout);
};

#endif