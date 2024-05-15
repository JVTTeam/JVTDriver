#pragma once

#include <string>
#include <functional>
#include <memory>

class Logger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

public:
    static Logger &GetInstance()
    {
        static Logger instance;
        return instance;
    }

    void Subscribe(std::function<void(const std::string &, Level level)> onReceive)
    {
        m_Subscribers.push_back(onReceive);
    }

    template <typename... Args>
    void Log(Level level, const std::string &format, Args... args)
    {
        const std::string message = Format(format, args...);

        for (const auto &subscriber : m_Subscribers)
            subscriber(message, level);
    }

private:
    Logger() = default;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    template <typename... Args>
    static std::string Format(const std::string &format, Args... args)
    {
        // Get the size of the formatted string
        const std::size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
        if (size <= 0)
            return format;
        
        // Allocate a buffer and format the string
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
        snprintf(buffer.get(), size, format.c_str(), args...);
        std::string str =  std::string(buffer.get(), buffer.get() + size - 1);

        // Remove trailing whitespace
        str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), str.end());
        return str;
    }

private:
    std::vector<std::function<void(const std::string &, Level level)>> m_Subscribers;
};