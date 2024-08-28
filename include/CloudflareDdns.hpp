#ifndef CLOUDFLARE_DDNS_HPP
#define CLOUDFLARE_DDNS_HPP

#include "DdnsApiTrait.hpp"
#include "pch.hpp"

class CF_DDNS : public DDNS_API
{
  public:
    virtual std::string getRecordIp() override;
    virtual std::string getLocalIp() override;
    virtual std::string setRecordIp(const std::string &ip) override;
    CF_DDNS(IpVersion version = IpVersion::V4);

  private:
    httplib::Client cloudflareClient_;
};

#endif