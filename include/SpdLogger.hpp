#ifndef SPD_LOGGER_HPP
#define SPD_LOGGER_HPP

#include "ILogger.hpp"
#include <memory>
#include <spdlog/spdlog.h>

class SpdLogger : public ILogger
{
  public:
    SpdLogger();
    void info(const std::string &msg) override;
    void warning(const std::string &msg) override;
    void error(const std::string &msg) override;

  private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#endif