#include <cstddef>
#include <fmt/printf.h>

#include "LocalRecordDifferTrait.hpp"

void SeeIpInfo::setIpInfo(const json &j)
{
    country = j.contains("country_code") ? std::make_optional(j["country_code"].get<std::string>()) : std::nullopt;
    countryRegion = std::nullopt;
    region = std::nullopt;
    city = std::nullopt;
    asOrganization =
        j.contains("organization") ? std::make_optional(j["organization"].get<std::string>()) : std::nullopt;
    asn = j.contains("asn") ? std::make_optional(j["asn"].get<size_t>()) : std::nullopt;
    latitude = j.contains("latitude") ? std::make_optional(j["latitude"].get<double>()) : std::nullopt;
    longitude = j.contains("longitude") ? std::make_optional(j["longitude"].get<double>()) : std::nullopt;
}

LRDiffer_API::LRDiffer_API(IpVersion version, const std::string &getIpApi)
    : getLocalClient_(getIpApi), DDNS_API(version)
{
    auto &[info, isRenew] = ipInfo;
    info = SeeIpInfo();
    isRenew = false;
    switch (version)
    {
    case IpVersion::V4:
        getLocalClient_.set_address_family(AF_INET6);
        break;
    case IpVersion::V6:
        getLocalClient_.set_address_family(AF_INET);
        break;
    }
}

std::string LRDiffer_API::getLocalIp()
{
    auto result = getLocalClient_.Get("/geoip");
    if (result.error() != httplib::Error::Success)
    {
        return "Http Error";
    }
    json j;
    std::string ip;
    auto &[info, isRenew] = ipInfo;
    try
    {
        j = json::parse(result.value().body);
        ip = j["ip"].get<std::string>();
    }
    catch (json::parse_error &error)
    {
        fmt::println("\033[0;31mParse error: {}\033[0m", error.what());
        ip = error.what();
    }
    catch (json::out_of_range &error)
    {
        fmt::println("\033[0;31mOut of range error: {}\033[0m", error.what());
        ip = error.what();
    }
    catch (std::exception &error)
    {
        fmt::println("\033[0;31mGeneral error:: {}\033[0m", error.what());
        ip = error.what();
    }
    return ip;
}