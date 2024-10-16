#ifndef WINDOWS_SERVICE_HPP
#define WINDOWS_SERVICE_HPP

#ifdef __WIN32__

#include "ISystemService.hpp"
#include <string>

class WindowsService : public ISystemService
{
  public:
    WindowsService(const std::string &serviceName = "AkashaDDNSService");
    ~WindowsService();

    bool enable(const std::string &configFilePath) override;
    bool disable() override;
    bool start() override;
    bool stop() override;
    bool showLogs(int numberOfLines = 100) override;

  private:
    std::string m_serviceName;

    // Helper methods
    std::string getExecutablePath() const;
    std::string getLogFilePath() const;
};

#endif // __WIN32__

#endif