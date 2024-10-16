#ifdef __WIN32__

#include "WindowsService.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <windows.h>

// Helper function to execute a command and capture output
std::vector<std::string> executeCommand(const std::string &cmd)
{
    std::vector<std::string> output;
    char buffer[128];
    FILE *pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
        return output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        output.emplace_back(buffer);
    }
    _pclose(pipe);
    return output;
}

WindowsService::WindowsService(const std::string &serviceName) : m_serviceName(serviceName)
{
}

WindowsService::~WindowsService()
{
}

bool WindowsService::enable(const std::string &configFilePath)
{
    std::string exePath = getExecutablePath();
    std::string cmd = "sc create \"" + m_serviceName + "\" binPath= \"" + exePath + " --run-service --config \"" +
                      configFilePath + "\"\" start= auto";
    std::vector<std::string> result = executeCommand(cmd);
    return !result.empty();
}

bool WindowsService::disable()
{
    std::string cmd = "sc delete \"" + m_serviceName + "\"";
    std::vector<std::string> result = executeCommand(cmd);
    return !result.empty();
}

bool WindowsService::start()
{
    std::string cmd = "sc start \"" + m_serviceName + "\"";
    std::vector<std::string> result = executeCommand(cmd);
    return !result.empty();
}

bool WindowsService::stop()
{
    std::string cmd = "sc stop \"" + m_serviceName + "\"";
    std::vector<std::string> result = executeCommand(cmd);
    return !result.empty();
}

bool WindowsService::showLogs(int numberOfLines)
{
    std::string logFile = getLogFilePath();
    std::ifstream file(logFile);
    if (!file.is_open())
    {
        return false;
    }

    // Read all lines into a vector
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        lines.emplace_back(line);
    }
    file.close();

    // Display the last 'numberOfLines' lines
    int start = lines.size() > numberOfLines ? lines.size() - numberOfLines : 0;
    for (size_t i = start; i < lines.size(); ++i)
    {
        std::cout << lines[i] << std::endl;
    }
    return true;
}

std::string WindowsService::getExecutablePath() const
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::string(path);
}

std::string WindowsService::getLogFilePath() const
{
    // Define log file path, e.g., in the same directory as the executable
    std::string exePath = getExecutablePath();
    size_t pos = exePath.find_last_of("\\/");
    std::string dir = (pos != std::string::npos) ? exePath.substr(0, pos) : ".";
    return dir + "\\AkashaDDNS.log";
}

#endif // __WIN32__
