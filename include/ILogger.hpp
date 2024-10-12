#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include <string>

class ILogger
{
  public:
    virtual ~ILogger() = default;
    virtual void info(const std::string &msg) = 0;
    virtual void warning(const std::string &msg) = 0;
    virtual void error(const std::string &msg) = 0;
};

#endif