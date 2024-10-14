#ifndef SERVICE_WINDOWS_HPP
#define SERVICE_WINDOWS_HPP

#ifdef __LINUX__

#include "ILogger.hpp"
#include "IService.hpp"
#include <memory>
#include <string>

class ServiceLinux : public IService
{
  public:
    ServiceLinux(const std::string &serviceName, std::shared_ptr<ILogger> logger);
    ~ServiceLinux();

    void start() override;
    void stop() override;

  private:
    void daemonize();
    void run();
    static void signalHandler(int signum);

    std::string m_serviceName;
    std::shared_ptr<ILogger> m_loggerInstance;
    static bool m_running;
};

#endif // !_WIN32

#endif