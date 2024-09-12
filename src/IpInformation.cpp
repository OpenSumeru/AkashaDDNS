#include "IpInformation.hpp"
#include <optional>

void IpInfo::setIpInfo(const json &j)
{
    country = j.contains("country") ? std::make_optional(j["country"].get<std::string>()) : std::nullopt;
    countryRegion =
        j.contains("countryRegion") ? std::make_optional(j["countryRegion"].get<std::string>()) : std::nullopt;
    region = j.contains("region") ? std::make_optional(j["region"].get<std::string>()) : std::nullopt;
    city = j.contains("city") ? std::make_optional(j["city"].get<std::string>()) : std::nullopt;
    asOrganization =
        j.contains("asOrganization") ? std::make_optional(j["asOrganization"].get<std::string>()) : std::nullopt;
    latitude = j.contains("latitude") ? std::make_optional(j["latitude"].get<double>()) : std::nullopt;
    longitude = j.contains("longitude") ? std::make_optional(j["longitude"].get<double>()) : std::nullopt;
}

void IpInfo::printIpInfo()
{
}