#pragma once

#include <crow.h>

class Logger : public crow::ILogHandler {
public:
    using LogLevel = crow::LogLevel;
    Logger(LogLevel log_level);
    ~Logger();

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
