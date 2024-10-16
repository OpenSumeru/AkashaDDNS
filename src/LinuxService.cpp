#ifdef __linux__

#include "LinuxService.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>


// Helper function to execute a command and capture output
std::vector<std::string> executeCommand(const std::string &cmd)
{
    std::vector<std::string> output;
    char buffer[128];
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        output.emplace_back(buffer);
    }
    pclose(pipe);
    return output;
}

LinuxService::LinuxService(const std::string &serviceName) : m_serviceName(serviceName)
{
}

LinuxService::~LinuxService()
{
}

bool LinuxService::enable(const std::string &configFilePath)
{
    std::string exePath = getExecutablePath();
    std::string serviceFilePath = getServiceFilePath();
    std::string logFilePath = getLogFilePath();

    // Create systemd service file
    std::ofstream serviceFile(serviceFilePath);
    if (!serviceFile.is_open())
    {
        std::cerr << "Failed to create service file at " << serviceFilePath << std::endl;
        return false;
    }

    serviceFile << "[Unit]\n";
    serviceFile << "Description=AkashaDDNS Service\n";
    serviceFile << "After=network.target\n\n";
    serviceFile << "[Service]\n";
    serviceFile << "ExecStart=" << exePath << " --run-service --config " << configFilePath << "\n";
    serviceFile << "Restart=always\n";
    serviceFile << "User=root\n"; // Consider using a dedicated user
    serviceFile << "StandardOutput=append:" << logFilePath << "\n";
    serviceFile << "StandardError=append:" << logFilePath << "\n\n";
    serviceFile << "[Install]\n";
    serviceFile << "WantedBy=multi-user.target\n";

    serviceFile.close();

    // Reload systemd daemon to recognize new service
    executeCommand("systemctl daemon-reload");

    // Enable the service to start on boot
    std::string cmdEnable = "systemctl enable " + m_serviceName;
    executeCommand(cmdEnable);

    return true;
}

bool LinuxService::disable()
{
    // Disable the service
    std::string cmdDisable = "systemctl disable " + m_serviceName;
    executeCommand(cmdDisable);

    // Remove the service file
    std::string serviceFilePath = getServiceFilePath();
    if (remove(serviceFilePath.c_str()) != 0)
    {
        std::cerr << "Failed to remove service file at " << serviceFilePath << std::endl;
        return false;
    }

    // Reload systemd daemon
    executeCommand("systemctl daemon-reload");

    return true;
}

bool LinuxService::start()
{
    std::string cmdStart = "systemctl start " + m_serviceName;
    executeCommand(cmdStart);
    return true;
}

bool LinuxService::stop()
{
    std::string cmdStop = "systemctl stop " + m_serviceName;
    executeCommand(cmdStop);
    return true;
}

bool LinuxService::showLogs(int numberOfLines)
{
    std::string logFile = getLogFilePath();
    std::string cmd = "tail -" + std::to_string(numberOfLines) + " " + logFile;
    std::vector<std::string> output = executeCommand(cmd);
    for (const auto &line : output)
    {
        std::cout << line;
    }
    return true;
}

std::string LinuxService::getExecutablePath() const
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return (count != -1) ? std::string(result, count) : "";
}

std::string LinuxService::getServiceFilePath() const
{
    return "/etc/systemd/system/" + m_serviceName + ".service";
}

std::string LinuxService::getLogFilePath() const
{
    // Define log file path, e.g., /var/log/AkashaDDNS.log
    return "/var/log/" + m_serviceName + ".log";
}

#endif // !_WIN32
