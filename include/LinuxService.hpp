#ifndef LINUX_SERVICE_HPP
#define LINUX_SERVICE_HPP

#ifdef __linux__

#include "ISystemService.hpp"
#include <string>

class LinuxService : public ISystemService
{
  public:
    LinuxService(const std::string &serviceName = "AkashaDDNSService");
    ~LinuxService();

    bool enable(const std::string &configFilePath) override;
    bool disable() override;
    bool start() override;
    bool stop() override;
    bool showLogs(int numberOfLines = 100) override;

  private:
    std::string m_serviceName;

    // Helper methods
    std::string getExecutablePath() const;
    std::string getServiceFilePath() const;
    std::string getLogFilePath() const;
};

#endif // __linux__

#endif