#include "CustomLogger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

CustomLogger::CustomLogger(const std::string &logFilePath, const std::vector<std::string> &consoleKeywords)
    : m_consoleKeywords(consoleKeywords)
{
    m_logFile.open(logFilePath, std::ios::app);
    if (!m_logFile.is_open())
    {
        throw std::runtime_error("Failed to open log file: " + logFilePath);
    }

    // Initial log message
    log("INFO", "Logger initialized.");
}

CustomLogger::~CustomLogger()
{
    if (m_logFile.is_open())
    {
        log("INFO", "Logger shutting down.");
        m_logFile.close();
    }
}

void CustomLogger::info(const std::string &message)
{
    log("INFO", message);
}

void CustomLogger::warning(const std::string &message)
{
    log("WARN", message);
}

void CustomLogger::error(const std::string &message)
{
    log("ERROR", message);
}

void CustomLogger::log(const std::string &level, const std::string &message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms_part = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm buf;
#ifdef _WIN32
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif

    // Format time
    std::ostringstream timeStream;
    timeStream << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
               << ms_part.count();

    // Write to log file
    m_logFile << "[" << timeStream.str() << "] [" << level << "] " << message << std::endl;

    // Check for keywords and write to console if necessary
    if (containsKeyword(message))
    {
        std::cout << "[" << timeStream.str() << "] [" << level << "] " << message << std::endl;
    }
}

bool CustomLogger::containsKeyword(const std::string &message)
{
    for (const auto &kw : m_consoleKeywords)
    {
        if (message.find(kw) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}
