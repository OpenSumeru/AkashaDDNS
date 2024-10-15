#include "CloudflareService.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CloudflareService::CloudflareService(const std::string &token, const std::string &email,
                                     std::shared_ptr<ILogger> logger)
    : m_token(token), m_email(email), m_logger(logger)
{
}

std::string CloudflareService::getPublicIP()
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

std::string CloudflareService::getZoneID(const std::string &domain)
{
    // 调用 Cloudflare API 获取 Zone ID
    httplib::Client cli("https://api.cloudflare.com");
    std::string path = "/client/v4/zones?name=" + domain;

    httplib::Headers headers = {
        {"Authorization", "Bearer " + m_token}, {"X-Auth-Email", m_email}, {"Content-Type", "application/json"}};

    auto res = cli.Get(path.c_str(), headers);
    if (res && res->status == 200)
    {
        auto resJson = json::parse(res->body);
        if (resJson.value("success", false) && !resJson["result"].empty())
        {
            m_logger->info("Successfully retrieved Zone ID for domain: " + domain);
            return resJson["result"][0]["id"];
        }
    }
    m_logger->error("Failed to retrieve Zone ID for domain: " + domain);
    return "";
}

std::string CloudflareService::getRecordID(const std::string &zoneID, const std::string &recordName)
{
    // 调用 Cloudflare API 获取 Record ID
    httplib::Client cli("https://api.cloudflare.com");
    std::string path = "/client/v4/zones/" + zoneID + "/dns_records?type=A&name=" + recordName;

    httplib::Headers headers = {
        {"Authorization", "Bearer " + m_token}, {"X-Auth-Email", m_email}, {"Content-Type", "application/json"}};

    auto res = cli.Get(path.c_str(), headers);
    if (res && res->status == 200)
    {
        auto resJson = json::parse(res->body);
        if (resJson.value("success", false) && !resJson["result"].empty())
        {
            m_logger->info("Successfully retrieved Record ID for record: " + recordName);
            return resJson["result"][0]["id"];
        }
    }
    m_logger->error("Failed to retrieve Record ID for record: " + recordName);
    return "";
}

bool CloudflareService::updateDNSRecord(const std::string &zoneID, const std::string &recordID,
                                        const std::string &recordName, const std::string &ip)
{
    // 调用 Cloudflare API 更新 DNS 记录
    httplib::Client cli("https://api.cloudflare.com");
    std::string path = "/client/v4/zones/" + zoneID + "/dns_records/" + recordID;

    json body = {{"type", "A"}, {"name", recordName}, {"content", ip}, {"ttl", 1}, {"proxied", false}};

    std::string bodyStr = body.dump();

    httplib::Headers headers = {
        {"Authorization", "Bearer " + m_token}, {"X-Auth-Email", m_email}, {"Content-Type", "application/json"}};

    auto res = cli.Put(path.c_str(), headers, bodyStr, "application/json");
    if (res && res->status == 200)
    {
        auto resJson = json::parse(res->body);
        if (resJson.value("success", false))
        {
            m_logger->info("Successfully updated DNS record for record: " + recordName);
            return true;
        }
    }
    m_logger->error("Failed to update DNS record for record: " + recordName);
    return false;
}

std::string CloudflareService::getDNSRecordContent(const std::string &zoneID, const std::string &recordID)
{
    // 调用 Cloudflare API 获取 DNS 记录的内容（IP）
    httplib::Client cli("https://api.cloudflare.com");
    std::string path = "/client/v4/zones/" + zoneID + "/dns_records/" + recordID;

    httplib::Headers headers = {
        {"Authorization", "Bearer " + m_token}, {"X-Auth-Email", m_email}, {"Content-Type", "application/json"}};

    auto res = cli.Get(path.c_str(), headers);
    if (res && res->status == 200)
    {
        auto resJson = json::parse(res->body);
        if (resJson.value("success", false) && resJson["result"].contains("content"))
        {
            std::string currentIP = resJson["result"]["content"];
            m_logger->info("Retrieved current DNS record content (IP): " + currentIP);
            return currentIP;
        }
    }
    m_logger->error("Failed to retrieve DNS record content for record ID: " + recordID);
    return "";
}