#ifndef ISYSTEM_SERVICE_HPP
#define ISYSTEM_SERVICE_HPP

#include <string>

class ISystemService
{
  public:
    virtual ~ISystemService() = default;

    // Registers the application as a system service using the provided config file path
    virtual bool enable(const std::string &configFilePath) = 0;

    // Unregisters the application from being a system service
    virtual bool disable() = 0;

    // Starts the system service
    virtual bool start() = 0;

    // Stops the system service
    virtual bool stop() = 0;

    // Displays the recent logs of the service
    virtual bool showLogs(int numberOfLines = 100) = 0;
};

#endif