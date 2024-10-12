#include "AliyunProvider.hpp"

AliyunProvider::AliyunProvider(std::shared_ptr<IExternalService> externalService, std::shared_ptr<ILogger> logger)
    : m_externalService(externalService), m_logger(logger)
{
}

void AliyunProvider::updateDNS(const std::string &recordName, const std::string &domain, const std::string &ip)
{
    m_logger->info("Starting DNS update for record: " + recordName + " (domain: " + domain + ") to IP: " + ip);

    std::string zoneID = m_externalService->getZoneID(domain);
    if (zoneID.empty())
    {
        m_logger->error("Failed to retrieve Zone ID for domain: " + domain);
        return;
    }

    std::string recordID = m_externalService->getRecordID(zoneID, recordName);
    if (recordID.empty())
    {
        m_logger->error("Failed to retrieve Record ID for record: " + recordName);
        return;
    }

    bool success = m_externalService->updateDNSRecord(zoneID, recordID, recordName, ip);
    if (success)
    {
        m_logger->info("Successfully updated DNS record for record: " + recordName);
    }
    else
    {
        m_logger->error("Failed to update DNS record for record: " + recordName);
    }
}

std::string AliyunProvider::getCurrentDNSIP(const std::string &recordName, const std::string &domain)
{
    std::string zoneID = m_externalService->getZoneID(domain);
    if (zoneID.empty())
    {
        m_logger->error("Failed to retrieve Zone ID for domain: " + domain);
        return "";
    }

    std::string recordID = m_externalService->getRecordID(zoneID, recordName);
    if (recordID.empty())
    {
        m_logger->error("Failed to retrieve Record ID for record: " + recordName);
        return "";
    }

    // 获取当前 DNS 记录的 IP
    std::string currentIP = m_externalService->getDNSRecordContent(zoneID, recordID);
    if (currentIP.empty())
    {
        m_logger->error("Failed to retrieve DNS record content for record: " + recordName);
    }
    else
    {
        m_logger->info("Current DNS IP for " + recordName + ": " + currentIP);
    }

    return currentIP;
}