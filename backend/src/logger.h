#pragma once

#include <crow.h>

class Logger : public crow::ILogHandler {
public:
    using LogLevel = crow::LogLevel;

    Logger(LogLevel log_level);
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger operator=(const Logger& other) = delete;

    static Logger& get_instance() {
        // TODO: set log level from env
        static Logger instance(LogLevel::Debug);
        return instance;
    }

    void log(const std::string& message, LogLevel level) override;

    void critical(const char* message);
    void error(const char* message);
    void warn(const char* message);
    void info(const char* message);
    void debug(const char* message);
private:
    FILE* m_logfile;
    LogLevel m_log_level;
};
