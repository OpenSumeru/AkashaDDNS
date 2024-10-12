#ifndef ALIYUN_PROVIDER_HPP
#define ALIYUN_PROVIDER_HPP

#include "IExternalService.hpp"
#include "ILogger.hpp"
#include "IProvider.hpp"
#include <memory>

class AliyunProvider : public IProvider
{
  public:
    AliyunProvider(std::shared_ptr<IExternalService> externalService, std::shared_ptr<ILogger> logger);
    void updateDNS(const std::string &recordName, const std::string &domain, const std::string &ip) override;
    std::string getCurrentDNSIP(const std::string &recordName, const std::string &domain) override;

  private:
    std::shared_ptr<IExternalService> m_externalService;
    std::shared_ptr<ILogger> m_logger;
};

#endif