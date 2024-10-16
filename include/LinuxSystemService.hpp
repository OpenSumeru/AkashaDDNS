#ifndef LINUX_SERVICE_HPP
#define LINUX_SERVICE_HPP

#ifdef __linux__

#include "ILogger.hpp"
#include "ISystemService.hpp"
#include <memory>
#include <string>

class LinuxSystemService : public ISystemService
{
  public:
    LinuxSystemService(std::shared_ptr<ILogger> logger);
    ~LinuxSystemService();

    void enable(const std::string &configFilePath) override;
    void disable() override;
    void start() override;
    void stop() override;
    void show() override;

  private:
    std::shared_ptr<ILogger> m_logger;
    std::string m_serviceName = "AkashaDDNSService";
    std::string m_executablePath;
    std::string m_configFilePath;
    std::string generateServiceFile() const;
};

#endif // !_WIN32

#endif