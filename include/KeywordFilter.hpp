#ifndef KEYWORD_FILTER_HPP
#define KEYWORD_FILTER_HPP

#include <spdlog/sinks/sink.h>
#include <string>
#include <vector>

class KeywordFilter : public spdlog::sinks::sink
{
  public:
    // 构造函数，接收一个包装的 Sink 和关键词列表
    KeywordFilter(std::shared_ptr<spdlog::sinks::sink> wrapped_sink, const std::vector<std::string> &keywords)
        : wrapped_sink_(wrapped_sink), keywords_(keywords)
    {
    }

    // 重写 log 方法
    void log(const spdlog::details::log_msg &msg) override
    {
        // 正确构造 std::string 从 spdlog::string_view_t
        std::string formatted(msg.payload.data(), msg.payload.size());

        // 检查是否包含任何关键词
        for (const auto &kw : keywords_)
        {
            if (formatted.find(kw) != std::string::npos)
            {
                wrapped_sink_->log(msg); // 转发到包装的 Sink
                return;
            }
        }
        // 不匹配任何关键词，忽略日志消息
    }

    // 重写 flush 方法
    void flush() override
    {
        wrapped_sink_->flush();
    }

    // 重写 set_pattern 方法
    void set_pattern(const std::string &pattern) override
    {
        wrapped_sink_->set_pattern(pattern);
    }

    // 重写 set_formatter 方法
    void set_formatter(std::unique_ptr<spdlog::formatter> formatter) override
    {
        wrapped_sink_->set_formatter(std::move(formatter));
    }

    // 代理其他方法到包装的 Sink
    void set_level(spdlog::level::level_enum level)
    {
        wrapped_sink_->set_level(level);
    }

    spdlog::level::level_enum level() const
    {
        return wrapped_sink_->level();
    }

    bool should_log(spdlog::level::level_enum msg_level) const
    {
        return wrapped_sink_->should_log(msg_level);
    }

  private:
    std::shared_ptr<spdlog::sinks::sink> wrapped_sink_; // 包装的 Sink（控制台 Sink）
    std::vector<std::string> keywords_;                 // 关键词列表
};

#endif