#include "AliyunProvider.hpp"
#include "AliyunService.hpp"
#include "CloudflareProvider.hpp"
#include "CloudflareService.hpp"
#include "ConfigManager.hpp"
#include "DDNSClient.hpp"
#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <memory>

#ifdef USE_SPDLOG
#include "SpdLogger.hpp"
#else
#include "CustomLogger.hpp"
#endif

#ifdef _WIN32
#include "WindowsSystemService.hpp"
#endif
#ifdef __linux__
#include "LinuxSystemService.hpp"
#endif

// Utility function to extract domain from record name
std::string extractDomain(const std::string &recordName)
{
    size_t pos = recordName.find('.');
    if (pos == std::string::npos)
    {
        return recordName; // If no dot found, return the original string
    }
    return recordName.substr(pos + 1);
}

// Global pointer to access DDNSClient in the signal handler
DDNSClient *g_ddnsClient = nullptr;

// Signal handler
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
        // Register signal handlers
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        // Check if the first argument is 'service'
        if (argc > 1 && std::string(argv[1]) == "service")
        {
            if (argc < 3)
            {
                std::cerr << "Service command requires an action: enable, disable, start, stop, show" << std::endl;
                return -1;
            }

            std::string action = argv[2];

            // Define console keywords
            std::vector<std::string> consoleKeywords = {"IP change", "Startup parameters"};

            // Create logger
#ifdef USE_SPDLOG
            std::shared_ptr<ILogger> logger = std::make_shared<SpdLogger>();
#else
            std::shared_ptr<ILogger> logger = std::make_shared<CustomLogger>("AkashaDDNS.log", consoleKeywords);
#endif

            // Instantiate the appropriate system service
            std::unique_ptr<ISystemService> systemService;

#ifdef _WIN32
            systemService = std::make_unique<WindowsSystemService>(logger);
#else
            systemService = std::make_unique<LinuxSystemService>(logger);
#endif

            if (action == "enable")
            {
                if (argc < 4)
                {
                    std::cerr << "Service enable requires a config file path." << std::endl;
                    return -1;
                }
                std::string configFilePath = argv[3];
                systemService->enable(configFilePath);
            }
            else if (action == "disable")
            {
                systemService->disable();
            }
            else if (action == "start")
            {
                systemService->start();
            }
            else if (action == "stop")
            {
                systemService->stop();
            }
            else if (action == "show")
            {
                systemService->show();
            }
            else
            {
                std::cerr << "Unknown service action: " << action << std::endl;
                return -1;
            }

            return 0;
        }

        // Parse command-line arguments
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

        // Load configuration
        ConfigManager configManager(configFile);
        if (!configManager.load())
        {
            return -1;
        }

        // Get configuration parameters, command-line arguments take precedence
        std::string providerName =
            result.count("provider") ? result["provider"].as<std::string>() : configManager.getProvider();
        std::string recordName =
            result.count("record-name") ? result["record-name"].as<std::string>() : configManager.getRecordName();
        std::string email = result.count("email") ? result["email"].as<std::string>() : configManager.getEmail();
        int updateInterval = configManager.getUpdateInterval(); // Get update interval

        // Extract domain from recordName
        std::string domain = extractDomain(recordName);
        if (domain.empty())
        {
            std::cerr << "Invalid record name: " << recordName << std::endl;
            return -1;
        }

        // Instantiate logger based on the defined macro
#ifdef USE_SPDLOG
        std::shared_ptr<ILogger> logger = std::make_shared<SpdLogger>();
#else
        std::vector<std::string> consoleKeywords = {"IP change", "Startup parameters"};
        std::shared_ptr<ILogger> logger = std::make_shared<CustomLogger>("AkashaDDNS.log", consoleKeywords);
#endif

        std::unique_ptr<IProvider> provider;

        if (providerName == "cloudflare")
        {
            std::string token =
                result.count("token") ? result["token"].as<std::string>() : configManager.getCloudflareToken();
            if (token.empty() || recordName.empty() || email.empty())
            { // Add email check
                logger->error("Token, record name, or email is missing for Cloudflare.");
                return -1;
            }
            auto externalService = std::make_shared<CloudflareService>(token, email, logger); // Pass email
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
            { // Add email check
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

        // Create and run client
        DDNSClient client(std::move(provider), logger, updateInterval);
        g_ddnsClient = &client; // Set global pointer

        client.run(recordName, domain);
    }
    catch (const cxxopts::exceptions::exception &e)
    { // Use correct namespace prefix
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