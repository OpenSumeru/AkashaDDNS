#ifndef ALIYUN_SERVICE_HPP
#define ALIYUN_SERVICE_HPP

#include "IExternalService.hpp"
#include "ILogger.hpp"
#include <memory>
#include <pch.hpp>
#include <string>


class AliyunService : public IExternalService
{
  public:
    AliyunService(const std::string &accessKeyId, const std::string &accessKeySecret, std::shared_ptr<ILogger> logger);

    std::string getPublicIP() override;
    std::string getZoneID(const std::string &domain) override;
    std::string getRecordID(const std::string &zoneID, const std::string &recordName) override;
    bool updateDNSRecord(const std::string &zoneID, const std::string &recordID, const std::string &recordName,
                         const std::string &ip) override;
    std::string getDNSRecordContent(const std::string &zoneID, const std::string &recordID) override; // 实现方法

  private:
    std::string m_accessKeyId;
    std::string m_accessKeySecret;
    std::shared_ptr<ILogger> m_logger;

    // 您可能需要添加其他成员变量，例如用于 API 请求的客户端等
};

#endif