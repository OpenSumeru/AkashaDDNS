// src/LinuxSystemService.cpp
#ifdef __linux__

#include "LinuxSystemService.hpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <vector>


LinuxSystemService::LinuxSystemService(std::shared_ptr<ILogger> logger) : m_logger(logger)
{
    // Get the path to the executable
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    if (count != -1)
    {
        m_executablePath = std::string(path, count);
    }
    else
    {
        m_logger->error("Failed to get executable path.");
    }
}

LinuxSystemService::~LinuxSystemService()
{
    // Destructor
}

std::string LinuxSystemService::generateServiceFile() const
{
    std::ostringstream serviceFile;
    serviceFile << "[Unit]\n"
                << "Description=AkashaDDNS Service\n"
                << "After=network.target\n\n"
                << "[Service]\n"
                << "ExecStart=" << m_executablePath << " --config " << m_configFilePath << "\n"
                << "Restart=always\n"
                << "User=nobody\n"
                << "Group=nogroup\n\n"
                << "[Install]\n"
                << "WantedBy=multi-user.target\n";
    return serviceFile.str();
}

void LinuxSystemService::enable(const std::string &configFilePath)
{
    m_configFilePath = configFilePath;

    // Generate service file content
    std::string serviceContent = generateServiceFile();

    // Write service file to /etc/systemd/system/
    std::string servicePath = "/etc/systemd/system/" + m_serviceName + ".service";
    std::ofstream serviceFile(servicePath);
    if (!serviceFile.is_open())
    {
        m_logger->error("Failed to write service file to " + servicePath + ". Please run as root.");
        return;
    }
    serviceFile << serviceContent;
    serviceFile.close();

    m_logger->info("Service file written to " + servicePath);

    // Reload systemd daemon
    int ret = system("systemctl daemon-reload");
    if (ret != 0)
    {
        m_logger->error("Failed to reload systemd daemon. Command exited with code " + std::to_string(ret));
        return;
    }

    m_logger->info("Systemd daemon reloaded.");

    // Enable the service
    std::ostringstream enableCmd;
    enableCmd << "systemctl enable " << m_serviceName;
    ret = system(enableCmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to enable service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service enabled successfully.");
    }
}

void LinuxSystemService::disable()
{
    // Stop the service first
    stop();

    // Disable the service
    std::ostringstream disableCmd;
    disableCmd << "systemctl disable " << m_serviceName;
    int ret = system(disableCmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to disable service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service disabled successfully.");
    }

    // Remove the service file
    std::string servicePath = "/etc/systemd/system/" + m_serviceName + ".service";
    ret = remove(servicePath.c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to remove service file: " + servicePath + ". Please remove it manually if necessary.");
    }
    else
    {
        m_logger->info("Service file removed successfully.");
    }

    // Reload systemd daemon
    ret = system("systemctl daemon-reload");
    if (ret != 0)
    {
        m_logger->error("Failed to reload systemd daemon. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Systemd daemon reloaded.");
    }
}

void LinuxSystemService::start()
{
    // Start the service
    std::ostringstream startCmd;
    startCmd << "systemctl start " << m_serviceName;
    int ret = system(startCmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to start service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service started successfully.");
    }
}

void LinuxSystemService::stop()
{
    // Stop the service
    std::ostringstream stopCmd;
    stopCmd << "systemctl stop " << m_serviceName;
    int ret = system(stopCmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to stop service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service stopped successfully.");
    }
}

void LinuxSystemService::show()
{
    // Assuming that logs are written to AkashaDDNS.log in the executable's directory
    // Get the executable directory
    size_t pos = m_executablePath.find_last_of("/\\");
    std::string logFilePath =
        (pos != std::string::npos) ? m_executablePath.substr(0, pos + 1) + "AkashaDDNS.log" : "AkashaDDNS.log";

    std::ifstream logFile(logFilePath);
    if (!logFile.is_open())
    {
        m_logger->error("Failed to open log file: " + logFilePath);
        return;
    }

    std::string line;
    // Show the last 100 lines
    std::vector<std::string> lines;
    while (std::getline(logFile, line))
    {
        lines.push_back(line);
        if (lines.size() > 100)
        {
            lines.erase(lines.begin());
        }
    }
    logFile.close();

    m_logger->info("Recent logs:");
    for (const auto &l : lines)
    {
        std::cout << l << std::endl;
    }
}

#endif // !_WIN32
