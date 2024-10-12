#include "ConfigManager.hpp"
#include <fstream>
#include <iostream>

ConfigManager::ConfigManager(const std::string &configFile) : m_configFile(configFile)
{
}

bool ConfigManager::load()
{
    std::ifstream ifs(m_configFile);
    if (!ifs.is_open())
    {
        std::cerr << "Failed to open config file: " << m_configFile << std::endl;
        return false;
    }

    try
    {
        ifs >> m_config;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Config file parsing error: " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string ConfigManager::getProvider() const
{
    return m_config.value("provider", "cloudflare");
}

// Cloudflare
std::string ConfigManager::getCloudflareToken() const
{
    return m_config.value("token", "");
}

// Aliyun
std::string ConfigManager::getAliyunAccessKeyId() const
{
    return m_config.value("access_key_id", "");
}

std::string ConfigManager::getAliyunAccessKeySecret() const
{
    return m_config.value("access_key_secret", "");
}

std::string ConfigManager::getAliyunRecordId() const
{
    return m_config.value("record_id", "");
}

std::string ConfigManager::getAliyunZoneId() const
{
    return m_config.value("zone_id", "");
}

// 通用获取
std::string ConfigManager::getRecordName() const
{
    return m_config.value("record_name", "");
}

std::string ConfigManager::getEmail() const
{
    return m_config.value("email", "");
}

int ConfigManager::getUpdateInterval() const
{
    return m_config.value("update_interval", 300); // 默认值为300秒
}