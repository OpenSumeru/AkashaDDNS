#ifdef __WIN32__

#include "WindowsService.hpp"
#include <iostream>
#include <string>
#include <windows.h>

// Global service name
const std::string SERVICE_NAME = "AkashaDDNSService";
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

// Forward declarations
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);

// Application logic when running as a service
void runService(const std::string &configFilePath)
{
    // Initialize logger with config file
    // Initialize AkashaDDNS client
    // Start DDNSClient run loop
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandlerA(SERVICE_NAME.c_str(), ServiceCtrlHandler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0)
    {
        return;
    }

    // Report running status
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    // Parse command-line arguments for config file
    std::string configFilePath = "config.json"; // Default config
    for (DWORD i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc)
        {
            configFilePath = argv[i + 1];
            ++i;
        }
    }

    // Run the service logic
    runService(configFilePath);

    // Report stopped status
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hStatus, &ServiceStatus);
}

void WINAPI ServiceCtrlHandler(DWORD ctrlCode)
{
    switch (ctrlCode)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hStatus, &ServiceStatus);
        // Signal the service to stop
        break;
    default:
        break;
    }
}

#endif // __WIN32__
