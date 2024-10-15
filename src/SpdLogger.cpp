#ifdef USE_SPDLOG

#include "SpdLogger.hpp"
#include "KeywordFilter.hpp"
#include <iostream>
#include <pch.hpp>
#include <string>
#include <vector>

SpdLogger::SpdLogger()
{
    try
    {
        // 创建文件 Sink，记录所有日志级别
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("AkashaDDNS.log", true);
        file_sink->set_level(spdlog::level::trace); // 记录所有级别

        // 创建控制台 Sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info); // 设置最低级别为 info

        // 定义需要输出到控制台的关键词
        std::vector<std::string> keywords = {"IP change", "Startup parameters"};

        // 创建带有关键词过滤的控制台 Sink
        auto filtered_console_sink = std::make_shared<KeywordFilter>(console_sink, keywords);

        // 创建 logger，包含文件 Sink 和过滤后的控制台 Sink
        m_logger =
            std::make_shared<spdlog::logger>("AkashaDDNS", spdlog::sinks_init_list{file_sink, filtered_console_sink});
        m_logger->set_level(spdlog::level::trace);            // 设置全局日志级别
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v"); // 设置日志格式

        // 注册 logger
        spdlog::register_logger(m_logger);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

void SpdLogger::info(const std::string &message)
{
    m_logger->info(message);
}

void SpdLogger::error(const std::string &message)
{
    m_logger->error(message);
}

void SpdLogger::warning(const std::string &message)
{
    m_logger->warn(message);
}

#endif