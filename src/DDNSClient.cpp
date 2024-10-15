#include "DDNSClient.hpp"
#include <chrono>
#include <pch.hpp>
#include <thread>

using json = nlohmann::json;

DDNSClient::DDNSClient(std::unique_ptr<IProvider> provider, std::shared_ptr<ILogger> logger, int updateInterval)
    : m_provider(std::move(provider)), m_logger(logger), m_updateInterval(updateInterval), m_running(true)
{
}

void DDNSClient::run(const std::string &recordName, const std::string &domain)
{
    // 记录启动参数，包含关键词 "Startup parameters"
    m_logger->info("Startup parameters: provider=" + m_provider->getProviderName() + ", record_name=" + recordName +
                   ", domain=" + domain);

    while (m_running)
    {
        std::string ipInfo = getPublicIP();
        if (!ipInfo.empty())
        {
            try
            {
                auto ipData = json::parse(ipInfo);
                std::string currentIP = ipData["query"]; // 根据 ip-api.com 的返回结构
                m_logger->info("Public IP: " + currentIP);
                m_logger->info("Country: " + ipData.value("country", "Unknown"));
                m_logger->info("Region: " + ipData.value("regionName", "Unknown"));
                m_logger->info("City: " + ipData.value("city", "Unknown"));

                // 获取当前 DNS 记录的 IP
                std::string currentDNSIP = m_provider->getCurrentDNSIP(recordName, domain);
                if (currentDNSIP.empty())
                {
                    m_logger->error("Failed to retrieve current DNS IP for record: " + recordName);
                }
                else
                {
                    m_logger->info("Current DNS IP: " + currentDNSIP);
                    if (currentIP != currentDNSIP)
                    {
                        // 记录 IP 更改，包含关键词 "IP change"
                        m_logger->info("IP change detected: old_ip=" + currentDNSIP + ", new_ip=" + currentIP);
                        m_provider->updateDNS(recordName, domain, currentIP);
                    }
                    else
                    {
                        m_logger->info("IP is up-to-date. No update needed.");
                    }
                }
            }
            catch (const json::parse_error &e)
            {
                m_logger->error("Failed to parse IP information: " + std::string(e.what()));
            }
            catch (const std::exception &e)
            {
                m_logger->error("Unexpected error: " + std::string(e.what()));
            }
        }
        else
        {
            m_logger->error("Failed to retrieve public IP information.");
        }

        m_logger->info("Sleeping for " + std::to_string(m_updateInterval) + " seconds...");
        std::this_thread::sleep_for(std::chrono::seconds(m_updateInterval));
    }

    m_logger->info("DDNSClient stopped.");
}

void DDNSClient::stop()
{
    m_running = false;
}

std::string DDNSClient::getPublicIP()
{
    // 使用 ip-api.com 获取公共 IP 和位置信息
    httplib::Client cli("http://ip-api.com");
    auto res = cli.Get("/json");
    if (res && res->status == 200)
    {
        m_logger->info("Successfully retrieved public IP information from ip-api.com.");
        return res->body;
    }
    m_logger->error("Failed to retrieve public IP information from ip-api.com.");
    return "";
}