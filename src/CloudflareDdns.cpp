#include <fmt/printf.h>

#include "CloudflareDdns.hpp"

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

std::string CF_DDNS::getRecordIp()
{
    return "1";
}

std::string CF_DDNS::getLocalIp()
{
    auto result = cloudflareIpClient_.Get("/geo");
    if (result.error() != httplib::Error::Success)
    {
        return "Http Error";
    }
    try
    {
        json j = json::parse(result.value().body);
        auto &[info, isRenew] = ipInfo;
        info.setIpInfo(j);
        isRenew = true;
        return j["ip"].get<std::string>();
    }
    catch (json::parse_error &error)
    {
        fmt::println("\033[0;31mParse error: {}\033[0m", error.what());
    }
    catch (json::out_of_range &error)
    {
        fmt::println("\033[0;31mOut of range error: {}\033[0m", error.what());
    }
    catch (std::exception &error)
    {
        fmt::println("\033[0;31mGeneral error:: {}\033[0m", error.what());
    }
    return "Json Error";
}

std::string CF_DDNS::setRecordIp(const std::string &ip)
{
    return "2";
}

bool CF_DDNS::verifyApi()
{
    auto result = cloudflareClient_.Get("/client/v4/user/tokens/verify");
    if (result.error() != httplib::Error::Success)
    {
        return false;
    }
    try
    {
        json j = json::parse(result.value().body);

        for (auto rax : j["messages"])
        {
            fmt::println("Code: {}; Message: {};", rax["code"].get<size_t>(), rax["message"].get<std::string>());
        }
        if (j["success"].get<bool>())
        {
            return true;
        }
        for (auto rax : j["errors"])
        {
            fmt::println("\033[0;31mError: {}\033[0m", rax.dump());
        }
    }
    catch (json::parse_error &error)
    {
        fmt::println("\033[0;31mParse error: {}\033[0m", error.what());
    }
    catch (json::out_of_range &error)
    {
        fmt::println("\033[0;31mOut of range error: {}\033[0m", error.what());
    }
    catch (std::exception &error)
    {
        fmt::println("\033[0;31mGeneral error:: {}\033[0m", error.what());
    }
    return false;
}

CF_DDNS::CF_DDNS(CF_API cloudflareApi, IpVersion version)
    : DDNS_API{version}, cloudflareClient_("https://api.cloudflare.com"),
      cloudflareIpClient_("https://cloudflare-ip.html.zone")
{
    auto &[info, isRenew] = ipInfo;
    info = CfIpInfo();
    isRenew = false;
    switch (version)
    {
    case IpVersion::V4:
        cloudflareClient_.set_address_family(AF_INET6);
        cloudflareIpClient_.set_address_family(AF_INET6);
        break;
    case IpVersion::V6:
        cloudflareClient_.set_address_family(AF_INET);
        cloudflareIpClient_.set_address_family(AF_INET);
        break;
    }
    cloudflareClient_.set_keep_alive(true);
    cloudflareIpClient_.set_keep_alive(true);
    httplib::Headers header;
    cloudflareApi.addAuthHeader(header);
    cloudflareClient_.set_default_headers(header);
}