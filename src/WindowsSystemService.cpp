// src/WindowsSystemService.cpp
#ifdef _WIN32

#include "WindowsSystemService.hpp"
#include <cstdlib>
#include <filesystem> // C++17 filesystem
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <windows.h>

namespace fs = std::filesystem;

WindowsSystemService::WindowsSystemService(std::shared_ptr<ILogger> logger) : m_logger(logger)
{
    // Get the path to the executable
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0)
    {
        throw std::runtime_error("Failed to get executable path.");
    }
    m_executablePath = std::string(path);
}

WindowsSystemService::~WindowsSystemService()
{
    // Destructor
}

void WindowsSystemService::enable(const std::string &configFilePath)
{
    m_configFilePath = configFilePath;

    try
    {
        // Get the executable directory
        fs::path exePath(m_executablePath);
        fs::path exeDir = exePath.parent_path();

        // Define the destination config path
        fs::path destConfigPath = exeDir / "config.json";

        // Copy the config file to the executable directory as config.json
        fs::copy_file(configFilePath, destConfigPath, fs::copy_options::overwrite_existing);

        m_logger->info("Configuration file copied to " + destConfigPath.string());
    }
    catch (const fs::filesystem_error &e)
    {
        m_logger->error("Failed to copy configuration file: " + std::string(e.what()));
        return;
    }

    // Escape backslashes in path
    std::string escapedExecutablePath = m_executablePath;
    size_t pos = 0;
    while ((pos = escapedExecutablePath.find("\\", pos)) != std::string::npos)
    {
        escapedExecutablePath.replace(pos, 1, "\\\\");
        pos += 2;
    }

    // Build the command to create the service without parameters
    std::ostringstream cmd;
    cmd << "sc create " << m_serviceName << " binPath= \"" << escapedExecutablePath << "\" start= auto";

    m_logger->info("Creating service with command: " + cmd.str());

    int ret = system(cmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to create service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service created successfully.");
    }
}

void WindowsSystemService::disable()
{
    // Stop the service first
    stop();

    // Build the command to delete the service
    std::ostringstream cmd;
    cmd << "sc delete " << m_serviceName;

    m_logger->info("Deleting service with command: " + cmd.str());

    int ret = system(cmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to delete service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service deleted successfully.");
    }
}

void WindowsSystemService::start()
{
    // Build the command to start the service
    std::ostringstream cmd;
    cmd << "sc start " << m_serviceName;

    m_logger->info("Starting service with command: " + cmd.str());

    int ret = system(cmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to start service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service started successfully.");
    }
}

void WindowsSystemService::stop()
{
    // Build the command to stop the service
    std::ostringstream cmd;
    cmd << "sc stop " << m_serviceName;

    m_logger->info("Stopping service with command: " + cmd.str());

    int ret = system(cmd.str().c_str());
    if (ret != 0)
    {
        m_logger->error("Failed to stop service. Command exited with code " + std::to_string(ret));
    }
    else
    {
        m_logger->info("Service stopped successfully.");
    }
}

void WindowsSystemService::show()
{
    // Assuming that logs are written to AkashaDDNS.log in the executable's directory
    try
    {
        fs::path exePath(m_executablePath);
        fs::path exeDir = exePath.parent_path();
        fs::path logFilePath = exeDir / "AkashaDDNS.log";

        if (!fs::exists(logFilePath))
        {
            m_logger->error("Log file does not exist: " + logFilePath.string());
            return;
        }

        std::ifstream logFile(logFilePath);
        if (!logFile.is_open())
        {
            m_logger->error("Failed to open log file: " + logFilePath.string());
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
    catch (const fs::filesystem_error &e)
    {
        m_logger->error("Filesystem error: " + std::string(e.what()));
    }
}

#endif // _WIN32
