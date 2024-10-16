#ifndef WINDOWS_SERVICE_HPP
#define WINDOWS_SERVICE_HPP

#include "ILogger.hpp"
#include "ISystemService.hpp"
#include <memory>
#include <string>

#ifdef _WIN32

class WindowsSystemService : public ISystemService
{
  public:
    WindowsSystemService(std::shared_ptr<ILogger> logger);
    ~WindowsSystemService();

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
};

#endif // _WIN32

#endif