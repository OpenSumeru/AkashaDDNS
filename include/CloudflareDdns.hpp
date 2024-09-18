#ifndef CLOUDFLARE_DDNS_HPP
#define CLOUDFLARE_DDNS_HPP

#include <stdexcept>

#include "DdnsApiTrait.hpp"
#include "pch.hpp"

struct CfIpInfo : IpInfo
{
    void setIpInfo(const json &j) override;
};

class CF_API
{
  public:
    virtual void addAuthHeader(httplib::Headers &header)
    {
        throw std::logic_error("Cloudflare api need to be inherited");
    }

  protected:
    std::string key_;
    std::string email_;
};

class CF_API_AUTH : public CF_API
{
  public:
    CF_API_AUTH(const std::string &apiKey, const std::string &email)
    {
        key_ = apiKey;
        email_ = email;
    }
    void addAuthHeader(httplib::Headers &header) override
    {
        header.insert({"X-Auth-Email", email_});
        header.insert({"Authorization", "Bearer " + key_});
    }
};

class CF_API_XKEY : public CF_API
{
  public:
    CF_API_XKEY(const std::string &apiKey, const std::string &email)
    {
        key_ = apiKey;
        email_ = email;
    }
    void addAuthHeader(httplib::Headers &header) override
    {
        header.insert({"X-Auth-Email", email_});
        header.insert({"X-Auth-Key", key_});
    }
};

class CF_DDNS : public DDNS_API
{
  public:
    std::string getRecordIp() override;
    std::string getLocalIp() override;
    std::string setRecordIp(const std::string &ip) override;
    bool verifyApi();
    CF_DDNS(CF_API cloudflareApi, IpVersion version = IpVersion::V4);

  private:
    httplib::Client cloudflareClient_;
    httplib::Client cloudflareIpClient_;
};

#endif