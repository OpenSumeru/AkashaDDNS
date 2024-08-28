#include <print>

#include "LocalRecordDifferTrait.hpp"

LRDiffer_API::LRDiffer_API(IpVersion version, const std::string &getIpApi)
    : getLocalClient_(getIpApi), DDNS_API(version)
{
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
    try
    {
        j = json::parse(result.value().body);
        ip = j["ip"];
    }
    catch (json::parse_error &error)
    {
        std::println("\033[0;31mParse error: {}\033[0m", error.what());
        ip = error.what();
    }
    catch (json::out_of_range &error)
    {
        std::println("\033[0;31mOut of range error: {}\033[0m", error.what());
        ip = error.what();
    }
    catch (std::exception &error)
    {
        std::println("\033[0;31mGeneral error:: {}\033[0m", error.what());
        ip = error.what();
    }
    return ip;
}