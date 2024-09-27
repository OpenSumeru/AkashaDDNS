#include "StructUsage/CloudflareStruct.hpp"

void CfIpInfo::setIpInfo(const json &j)
{
    country = j.contains("country") ? std::make_optional(j["country"].get<std::string>()) : std::nullopt;
    countryRegion =
        j.contains("countryRegion") ? std::make_optional(j["countryRegion"].get<std::string>()) : std::nullopt;
    region = j.contains("region") ? std::make_optional(j["region"].get<std::string>()) : std::nullopt;
    city = j.contains("city") ? std::make_optional(j["city"].get<std::string>()) : std::nullopt;
    asOrganization =
        j.contains("asOrganization") ? std::make_optional(j["asOrganization"].get<std::string>()) : std::nullopt;
    asn = std::nullopt;
    latitude = j.contains("latitude") ? std::make_optional(j["latitude"].get<double>()) : std::nullopt;
    longitude = j.contains("longitude") ? std::make_optional(j["longitude"].get<double>()) : std::nullopt;
}

CfRecordInfo::CfRecordInfo(std::string zone_name, std::string record_name,
                           std::optional<std::shared_ptr<httplib::Client>> getIdClient,
                           std::optional<std::string> zone_id, std::optional<std::string> record_id)
{
    zoneName = zone_name;
    recordName = record_name;
    if (!record_id.has_value() && !getIdClient.has_value())
    {
        throw std::invalid_argument("Need record id or get id client at least one to be valid");
    }
    if (zone_id.has_value())
    {
    }
}