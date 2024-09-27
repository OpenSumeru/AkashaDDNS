#ifndef CLOUDFLARE_STRUCT_HPP
#define CLOUDFLARE_STRUCT_HPP

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "../InfoStruct/IpInformation.hpp"
#include "../InfoStruct/RecordInfo.hpp"
#include "../pch.hpp"
#include "httplib.h"

struct CfRecordInfo : RecordInfo
{
    CfRecordInfo(std::string zone_name, std::string record_name,
                 std::optional<std::shared_ptr<httplib::Client>> getIdClient,
                 std::optional<std::string> zone_id = std::nullopt,
                 std::optional<std::string> record_id = std::nullopt);
};

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
    CF_API_AUTH(std::string apiKey, std::string email)
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
    CF_API_XKEY(std::string apiKey, std::string email)
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

#endif