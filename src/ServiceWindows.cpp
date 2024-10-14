#ifdef __WIN32__

#include "ServiceWindows.hpp"
#include <thread>

// Initialize static members
SERVICE_STATUS_HANDLE ServiceWindows::m_statusHandle = nullptr;
SERVICE_STATUS ServiceWindows::m_serviceStatus = {};
std::shared_ptr<ILogger> ServiceWindows::m_loggerInstance = nullptr;

// Global pointer to the service instance
static ServiceWindows *g_serviceInstance = nullptr;

ServiceWindows::ServiceWindows(const std::string &serviceName, std::shared_ptr<ILogger> logger)
    : m_serviceName(serviceName)
{
    m_loggerInstance = logger;
}

ServiceWindows::~ServiceWindows()
{
    // Destructor logic if needed
}

void ServiceWindows::start()
{
    g_serviceInstance = this;

    SERVICE_TABLE_ENTRY serviceTable[] = {{const_cast<char *>(m_serviceName.c_str()), ServiceWindows::serviceMain},
                                          {nullptr, nullptr}};

    if (!StartServiceCtrlDispatcher(serviceTable))
    {
        m_loggerInstance->error("Failed to start service control dispatcher.");
    }
}

void ServiceWindows::stop()
{
    // Implement stop logic if needed
    // Typically, you would signal the service to stop
}

void WINAPI ServiceWindows::serviceMain(DWORD argc, LPSTR *argv)
{
    m_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    m_serviceStatus.dwWin32ExitCode = NO_ERROR;
    m_serviceStatus.dwServiceSpecificExitCode = 0;
    m_serviceStatus.dwCheckPoint = 0;
    m_serviceStatus.dwWaitHint = 0;

    m_statusHandle =
        RegisterServiceCtrlHandlerA(g_serviceInstance->m_serviceName.c_str(), ServiceWindows::serviceCtrlHandler);
    if (!m_statusHandle)
    {
        g_serviceInstance->m_loggerInstance->error("RegisterServiceCtrlHandler failed.");
        return;
    }

    g_serviceInstance->reportStatus(SERVICE_START_PENDING);

    // Initialize service (e.g., start threads)
    g_serviceInstance->run();
}

void WINAPI ServiceWindows::serviceCtrlHandler(DWORD ctrlCode)
{
    switch (ctrlCode)
    {
    case SERVICE_CONTROL_STOP:
        m_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        ServiceWindows::m_loggerInstance->info("Service stop requested.");
        // Signal the service to stop
        // Implement your stop logic here
        ServiceWindows::m_serviceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceWindows::m_serviceStatus.dwWin32ExitCode = NO_ERROR;
        SetServiceStatus(ServiceWindows::m_statusHandle, &ServiceWindows::m_serviceStatus);
        break;
    default:
        break;
    }
}

void ServiceWindows::run()
{
    m_loggerInstance->info("Service started successfully.");
    reportStatus(SERVICE_RUNNING);

    // Main service loop
    while (m_serviceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        // Perform service tasks
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    m_loggerInstance->info("Service is stopping.");
}

void ServiceWindows::reportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint)
{
    m_serviceStatus.dwCurrentState = currentState;
    m_serviceStatus.dwWin32ExitCode = win32ExitCode;
    m_serviceStatus.dwWaitHint = waitHint;

    SetServiceStatus(m_statusHandle, &m_serviceStatus);
}

#endif // _WIN32
