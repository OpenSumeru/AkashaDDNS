#ifndef SERVICE_WINDOWS_HPP
#define SERVICE_WINDOWS_HPP

#ifdef __WIN32__

#include "ILogger.hpp"
#include "IService.hpp"
#include <memory>
#include <string>
#include <windows.h>

class ServiceWindows : public IService
{
  public:
    ServiceWindows(const std::string &serviceName, std::shared_ptr<ILogger> logger);
    ~ServiceWindows();

    void start() override;
    void stop() override;

  private:
    static void WINAPI serviceMain(DWORD argc, LPSTR *argv);
    static void WINAPI serviceCtrlHandler(DWORD ctrlCode);
    void run();
    void reportStatus(DWORD currentState, DWORD win32ExitCode = NO_ERROR, DWORD waitHint = 0);

    std::string m_serviceName;
    static SERVICE_STATUS_HANDLE m_statusHandle;
    static SERVICE_STATUS m_serviceStatus;
    static std::shared_ptr<ILogger> m_loggerInstance;
};

#endif // _WIN32

#endif