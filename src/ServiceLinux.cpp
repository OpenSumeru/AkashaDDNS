#ifdef __LINUX__

#include "ServiceLinux.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Initialize static member
bool ServiceLinux::m_running = false;

ServiceLinux::ServiceLinux(const std::string &serviceName, std::shared_ptr<ILogger> logger)
    : m_serviceName(serviceName), m_loggerInstance(logger)
{
}

ServiceLinux::~ServiceLinux()
{
    // Destructor logic if needed
}

void ServiceLinux::start()
{
    daemonize();
    m_running = true;
    run();
}

void ServiceLinux::stop()
{
    m_running = false;
    m_loggerInstance->info("Service stop requested.");
    // Implement additional stop logic if needed
}

void ServiceLinux::daemonize()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        m_loggerInstance->error("Failed to fork the process.");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        // Parent process exits
        exit(EXIT_SUCCESS);
    }

    // Child process continues
    if (setsid() < 0)
    {
        m_loggerInstance->error("Failed to create a new session.");
        exit(EXIT_FAILURE);
    }

    // Catch, ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, ServiceLinux::signalHandler);

    // Fork again to ensure the daemon cannot acquire a controlling terminal
    pid = fork();

    if (pid < 0)
    {
        m_loggerInstance->error("Failed to fork the process again.");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        // Parent process exits
        exit(EXIT_SUCCESS);
    }

    // Set new file permissions
    umask(0);

    // Change the working directory to the root directory
    if (chdir("/") < 0)
    {
        m_loggerInstance->error("Failed to change working directory to root.");
        exit(EXIT_FAILURE);
    }

    // Close all open file descriptors
    for (long x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
    {
        close(static_cast<int>(x));
    }

    // Redirect standard files to /dev/null
    open("/dev/null", O_RDWR); // stdin
    dup(0);                    // stdout
    dup(0);                    // stderr
}

void ServiceLinux::run()
{
    m_loggerInstance->info("Service started successfully.");

    // Main service loop
    while (m_running)
    {
        // Perform service tasks
        sleep(1);
    }

    m_loggerInstance->info("Service is stopping.");
}

void ServiceLinux::signalHandler(int signum)
{
    if (signum == SIGTERM)
    {
        ServiceLinux::m_running = false;
    }
}

#endif // !_WIN32
