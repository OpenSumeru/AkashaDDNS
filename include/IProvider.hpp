#ifndef IPROVIDER_HPP
#define IPROVIDER_HPP

#include <string>

class IProvider
{
  public:
    virtual ~IProvider() = default;
    virtual void updateDNS(const std::string &recordName, const std::string &domain, const std::string &ip) = 0;
    virtual std::string getCurrentDNSIP(const std::string &recordName, const std::string &domain) = 0; // 新增的函数

    virtual std::string getProviderName() const = 0; // 新增的方法
};

#endif