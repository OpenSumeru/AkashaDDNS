#include <format>
#include <iostream>

#include "InfoStruct/IpInformation.hpp"

void IpInfo::setIpInfo(const json &j)
{
    country = j.contains("country") ? std::make_optional(j["country"].get<std::string>()) : std::nullopt;
    countryRegion =
        j.contains("countryRegion") ? std::make_optional(j["countryRegion"].get<std::string>()) : std::nullopt;
    region = j.contains("region") ? std::make_optional(j["region"].get<std::string>()) : std::nullopt;
    city = j.contains("city") ? std::make_optional(j["city"].get<std::string>()) : std::nullopt;
    asOrganization =
        j.contains("asOrganization") ? std::make_optional(j["asOrganization"].get<std::string>()) : std::nullopt;
    asn = j.contains("asn") ? std::make_optional(j["asn"].get<size_t>()) : std::nullopt;
    latitude = j.contains("latitude") ? std::make_optional(j["latitude"].get<double>()) : std::nullopt;
    longitude = j.contains("longitude") ? std::make_optional(j["longitude"].get<double>()) : std::nullopt;
}

void IpInfo::printIpInfo(std::ostream &out)
{
    std::string address = "Address: ";
    if (country.has_value())
    {
        address += std::format("Country: {}", country.value());
    }
    if (countryRegion.has_value())
    {
        address += std::format("Country Region: {}", countryRegion.value());
    }
    if (region.has_value())
    {
        address += std::format("Region: {}", region.value());
    }
    if (city.has_value())
    {
        address += std::format("City: {}", city.value());
    }
    out << address << std::endl;
    std::string location = "Location: ";
    if (asOrganization.has_value())
    {
        location += std::format("asOrganization: {}", asOrganization.value());
    }
    if (asn.has_value())
    {
        location += std::format("asn: {}", asn.value());
    }
    if (latitude.has_value())
    {
        location += std::format("latitude: {}", latitude.value());
    }
    if (longitude.has_value())
    {
        location += std::format("longitude: {}", longitude.value());
    }
    out << location << std::endl;
}