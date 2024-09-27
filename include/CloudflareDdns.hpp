#ifndef CLOUDFLARE_DDNS_HPP
#define CLOUDFLARE_DDNS_HPP

#include "DdnsApiTrait.hpp"
#include "StructUsage/CloudflareStruct.hpp"

class CF_DDNS : public DDNS_API
{
  public:
    std::string getRecordIp() override;
    std::string getLocalIp() override;
    std::string setRecordIp(std::string ip) override;
    bool verifyApi();
    CF_DDNS(CF_API cloudflareApi, IpVersion version = IpVersion::V4);

  private:
    httplib::Client cloudflareClient_;
    httplib::Client cloudflareIpClient_;
};

#endif