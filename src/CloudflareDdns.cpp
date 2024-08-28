#include "CloudflareDdns.hpp"
#include "httplib.h"

std::string CF_DDNS::getRecordIp()
{
}
std::string CF_DDNS::getLocalIp()
{
}
std::string CF_DDNS::setRecordIp(const std::string &ip)
{
}
CF_DDNS::CF_DDNS(IpVersion version) : DDNS_API{version}, cloudflareClient_("https://api.cloudflare.com")
{
    switch (version)
    {
    case IpVersion::V4:
        cloudflareClient_.set_address_family(AF_INET6);
        break;
    case IpVersion::V6:
        cloudflareClient_.set_address_family(AF_INET);
        break;
    }
}