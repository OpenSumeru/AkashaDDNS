#ifndef SPD_LOGGER_HPP
#define SPD_LOGGER_HPP

#include "ILogger.hpp"
#include <memory>
#include <spdlog/spdlog.h>

class SpdLogger : public ILogger
{
  public:
    SpdLogger();
    void info(const std::string &message) override;
    void error(const std::string &message) override;
    void warning(const std::string &message) override;
    // 可根据需要添加更多方法，如 debug, critical 等
  private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#endif