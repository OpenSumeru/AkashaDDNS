#include "AliyunProvider.hpp"
#include "AliyunService.hpp"
#include "CloudflareProvider.hpp"
#include "CloudflareService.hpp"
#include "ConfigManager.hpp"
#include "DDNSClient.hpp"
#include "SpdLogger.hpp"
#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <memory>

std::string extractDomain(const std::string &recordName)
{
    size_t pos = recordName.find('.');
    if (pos == std::string::npos)
    {
        return recordName; // 如果没有点，返回原始字符串
    }
    return recordName.substr(pos + 1);
}

// 全局指针，用于在信号处理器中访问 DDNSClient 实例
DDNSClient *g_ddnsClient = nullptr;

// 信号处理器
void signalHandler(int signum)
{
    if (g_ddnsClient)
    {
        g_ddnsClient->stop();
    }
}

int main(int argc, char *argv[])
{
    try
    {
        // 注册信号处理器
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        // 解析命令行参数
        cxxopts::Options options("AkashaDDNS", "A DDNS client supporting multiple providers");
        options.add_options()("c,config", "Configuration file",
                              cxxopts::value<std::string>()->default_value("config.json"))(
            "p,provider", "DNS Provider (cloudflare, aliyun)",
            cxxopts::value<std::string>())("t,token", "API Token (for Cloudflare)", cxxopts::value<std::string>())(
            "a,access-key-id", "Access Key ID (for Aliyun)", cxxopts::value<std::string>())(
            "s,access-key-secret", "Access Key Secret (for Aliyun)", cxxopts::value<std::string>())(
            "r,record-name", "Record name to update (e.g., www.example.com)",
            cxxopts::value<std::string>())("e,email", "Email address for Cloudflare authentication",
                                           cxxopts::value<std::string>())("h,help", "Print usage");

        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        std::string configFile = result["config"].as<std::string>();

        // 加载配置
        ConfigManager configManager(configFile);
        if (!configManager.load())
        {
            return -1;
        }

        // 获取配置参数，命令行参数优先
        std::string providerName =
            result.count("provider") ? result["provider"].as<std::string>() : configManager.getProvider();
        std::string recordName =
            result.count("record-name") ? result["record-name"].as<std::string>() : configManager.getRecordName();
        std::string email = result.count("email") ? result["email"].as<std::string>() : configManager.getEmail();
        int updateInterval = configManager.getUpdateInterval(); // 获取更新间隔

        // 从 recordName 中提取 domain
        std::string domain = extractDomain(recordName);
        if (domain.empty())
        {
            std::cerr << "Invalid record name: " << recordName << std::endl;
            return -1;
        }

        std::shared_ptr<ILogger> logger = std::make_shared<SpdLogger>();

        std::unique_ptr<IProvider> provider;

        if (providerName == "cloudflare")
        {
            std::string token =
                result.count("token") ? result["token"].as<std::string>() : configManager.getCloudflareToken();
            if (token.empty() || recordName.empty() || email.empty())
            { // 添加 email 检查
                logger->error("Token, record name, or email is missing for Cloudflare.");
                return -1;
            }
            auto externalService = std::make_shared<CloudflareService>(token, email, logger); // 传递 email
            provider = std::make_unique<CloudflareProvider>(externalService, logger);
        }
        else if (providerName == "aliyun")
        {
            std::string accessKeyId = result.count("access-key-id") ? result["access-key-id"].as<std::string>()
                                                                    : configManager.getAliyunAccessKeyId();
            std::string accessKeySecret = result.count("access-key-secret")
                                              ? result["access-key-secret"].as<std::string>()
                                              : configManager.getAliyunAccessKeySecret();
            std::string recordId = configManager.getAliyunRecordId();
            std::string zoneId = configManager.getAliyunZoneId();

            if (accessKeyId.empty() || accessKeySecret.empty() || recordName.empty() || recordId.empty() ||
                zoneId.empty() || email.empty())
            { // 添加 email 检查
                logger->error("Access Key ID/Secret, record name, record ID, zone ID, or email is missing for Aliyun.");
                return -1;
            }

            auto externalService = std::make_shared<AliyunService>(accessKeyId, accessKeySecret, logger);
            provider = std::make_unique<AliyunProvider>(externalService, logger);
        }
        else
        {
            logger->error("Unsupported provider: " + providerName);
            return -1;
        }

        // 创建并运行客户端
        DDNSClient client(std::move(provider), logger, updateInterval);
        g_ddnsClient = &client; // 设置全局指针

        client.run(recordName, domain);
    }
    catch (const cxxopts::exceptions::exception &e)
    { // 使用正确的命名空间前缀
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}