#ifndef LOCAL_RECORD_DIFFER_TRAIT_HPP
#define LOCAL_RECORD_DIFFER_TRAIT_HPP

#include "DdnsApiTrait.hpp"
#include "pch.hpp"

struct SeeIpInfo : IpInfo
{
    void setIpInfo(const json &j) override;
};

class LRDiffer_API : public DDNS_API
{
  public:
    virtual std::string getLocalIp() override;
    LRDiffer_API(IpVersion version = IpVersion::V4, std::string getIpApi = "https://api.seeip.org");

  private:
    httplib::Client getLocalClient_;
};

#endif