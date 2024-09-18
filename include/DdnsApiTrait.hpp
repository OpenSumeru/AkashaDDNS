#ifndef DDNS_API_TRAIT_HPP
#define DDNS_API_TRAIT_HPP

#include <functional>
#include <string>
#include <utility>

#include "InfoStruct/IpInformation.hpp"
#include "ReuseFn/IpValidation.hpp"

enum class DDNS_API_Code
{
    NO_CHANGE,
    INVALID_IP,
    IP_CHANGED,
    SETTING_IP_ERROR,
    CHANGE_IP_ERROR
};

class DDNS_API
{
  public:
    virtual std::string getRecordIp() = 0;
    virtual std::string getLocalIp() = 0;
    virtual std::string setRecordIp(const std::string &ip) = 0;
    virtual void printIpInfo();
    DDNS_API(IpVersion version = IpVersion::V4);
    DDNS_API_Code ddnsCycle();
    std::function<bool(const std::string &)> isValidIp;

  protected:
    std::pair<IpInfo, bool> ipInfo;

  private:
    IpVersion version_;
};

#endif