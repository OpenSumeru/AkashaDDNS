#ifndef DDNS_CLIENT_HPP
#define DDNS_CLIENT_HPP

#include "ILogger.hpp"
#include "IProvider.hpp"
#include <atomic>
#include <memory>
#include <string>

class DDNSClient
{
  public:
    DDNSClient(std::unique_ptr<IProvider> provider, std::shared_ptr<ILogger> logger, int updateInterval);
    void run(const std::string &recordName, const std::string &domain);
    void stop(); // 新增的停止方法

  private:
    std::unique_ptr<IProvider> m_provider;
    std::shared_ptr<ILogger> m_logger;
    int m_updateInterval;
    std::atomic<bool> m_running; // 运行标志
    std::string getPublicIP();
};

#endif