#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <nlohmann/json.hpp>
#include <string>

class ConfigManager
{
  public:
    ConfigManager(const std::string &configFile);
    bool load();
    std::string getProvider() const;

    // Cloudflare
    std::string getCloudflareToken() const;

    // Aliyun
    std::string getAliyunAccessKeyId() const;
    std::string getAliyunAccessKeySecret() const;
    std::string getAliyunRecordId() const;
    std::string getAliyunZoneId() const;

    // 通用获取
    std::string getRecordName() const;
    std::string getEmail() const;
    int getUpdateInterval() const; // 新增的函数

  private:
    std::string m_configFile;
    nlohmann::json m_config;
};

#endif