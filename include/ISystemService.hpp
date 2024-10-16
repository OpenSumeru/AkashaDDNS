#ifndef ISYSTEM_SERVICE_HPP
#define ISYSTEM_SERVICE_HPP

#include <string>

class ISystemService
{
  public:
    virtual ~ISystemService() = default;

    // Enable the service with the path to the config file
    virtual void enable(const std::string &configFilePath) = 0;

    // Disable/unregister the service
    virtual void disable() = 0;

    // Start the service
    virtual void start() = 0;

    // Stop the service
    virtual void stop() = 0;

    // Show recent logs
    virtual void show() = 0;
};

#endif