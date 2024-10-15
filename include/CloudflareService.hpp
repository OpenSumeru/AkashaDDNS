#ifndef CLOUDFLARE_SERVICE_HPP
#define CLOUDFLARE_SERVICE_HPP

#include "IExternalService.hpp"
#include "ILogger.hpp"
#include <memory>
#include <pch.hpp>
#include <string>

class CloudflareService : public IExternalService
{
  public:
    CloudflareService(const std::string &token, const std::string &email, std::shared_ptr<ILogger> logger);

    std::string getPublicIP() override;
    std::string getZoneID(const std::string &domain) override;
    std::string getRecordID(const std::string &zoneID, const std::string &domain) override;
    bool updateDNSRecord(const std::string &zoneID, const std::string &recordID, const std::string &domain,
                         const std::string &ip) override;
    std::string getDNSRecordContent(const std::string &zoneID, const std::string &recordID) override;

  private:
    std::string m_token;
    std::string m_email; // 保存 email
    std::shared_ptr<ILogger> m_logger;
};

#endif