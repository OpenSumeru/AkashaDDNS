#ifndef CLOULDFLARE_PROVIDER_HPP
#define CLOULDFLARE_PROVIDER_HPP

#include "IExternalService.hpp"
#include "ILogger.hpp"
#include "IProvider.hpp"
#include <memory>

class CloudflareProvider : public IProvider
{
  public:
    CloudflareProvider(std::shared_ptr<IExternalService> externalService, std::shared_ptr<ILogger> logger);
    void updateDNS(const std::string &recordName, const std::string &domain, const std::string &ip) override;
    std::string getCurrentDNSIP(const std::string &recordName, const std::string &domain) override; // 实现方法

    std::string getProviderName() const override
    {
        return "Cloudflare";
    } // 实现方法

  private:
    std::shared_ptr<IExternalService> m_externalService;
    std::shared_ptr<ILogger> m_logger;
};

#endif