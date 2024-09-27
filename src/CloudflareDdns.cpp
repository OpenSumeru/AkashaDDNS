#include <fmt/printf.h>

#include "CloudflareDdns.hpp"
#include "fmt/base.h"

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

std::string CF_DDNS::setRecordIp(std::string ip)
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