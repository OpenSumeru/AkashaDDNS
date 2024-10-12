#include "SpdLogger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

SpdLogger::SpdLogger()
{
    m_logger = spdlog::stdout_color_mt("console");
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
}

void SpdLogger::info(const std::string &msg)
{
    m_logger->info(msg);
}

void SpdLogger::warning(const std::string &msg)
{
    m_logger->warn(msg);
}

void SpdLogger::error(const std::string &msg)
{
    m_logger->error(msg);
}
