#ifndef CUSTOM_LOGGER_HPP
#define CUSTOM_LOGGER_HPP

#include "ILogger.hpp"
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

class CustomLogger : public ILogger
{
  public:
    CustomLogger(const std::string &logFilePath, const std::vector<std::string> &consoleKeywords);
    ~CustomLogger();

    void info(const std::string &message) override;
    void warning(const std::string &message) override;
    void error(const std::string &message) override;

  private:
    void log(const std::string &level, const std::string &message);
    bool containsKeyword(const std::string &message);

    std::ofstream m_logFile;
    std::vector<std::string> m_consoleKeywords;
    std::mutex m_mutex;
};

#endif