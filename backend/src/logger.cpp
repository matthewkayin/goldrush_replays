#include "logger.h"

#include <cstdio>
#include <ctime>

Logger::Logger(LogLevel log_level) : m_log_level(log_level) {
    m_logfile = nullptr;

    char logfile_path[256];
    time_t _time = time(NULL);
    tm _tm = *localtime(&_time);
    sprintf(logfile_path, "./logs/%d-%02d-%02dT%02d%02d%02d.log",
            _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour,
            _tm.tm_min, _tm.tm_sec);

    m_logfile = fopen(logfile_path, "w");
    if (!m_logfile) {
        printf("Logger() unable to open logfile at path %s.\n", logfile_path);
    }
}

Logger::~Logger() {
    if (m_logfile) {
        fclose(m_logfile);
        m_logfile = nullptr;
    }
}

const char* log_level_str(Logger::LogLevel level) {
    switch (level) {
        case Logger::LogLevel::Critical:
            return "CRITICAL";
        case Logger::LogLevel::Error:
            return "ERROR   ";
        case Logger::LogLevel::Warning:
            return "WARNING ";
        case Logger::LogLevel::Info:
            return "INFO    ";
        case Logger::LogLevel::Debug:
            return "DEBUG   ";
    }
}

void Logger::log(const std::string& message, LogLevel level) {
    if (level < m_log_level) {
        return;
    }

    const size_t MESSAGE_BUFFER_LENGTH = 32000;
    char out_message[MESSAGE_BUFFER_LENGTH];

    time_t raw_time = time(NULL);
    tm time_info = *localtime(&raw_time);
    char* out_message_ptr = out_message;
    out_message_ptr += strftime(out_message_ptr, MESSAGE_BUFFER_LENGTH, "%Y-%m-%d %X", &time_info);

    out_message_ptr += sprintf(out_message_ptr, " [%s] %s\n", log_level_str(level), message.c_str());
    printf("%s", out_message);

    if (m_logfile) {
        fprintf(m_logfile, "%s", out_message);
    }
}

void Logger::critical(const char* message) { log(message, LogLevel::Critical); }
void Logger::error(const char* message) { log(message, LogLevel::Error); }
void Logger::warn(const char* message) { log(message, LogLevel::Warning); }
void Logger::info(const char* message) { log(message, LogLevel::Info); }
void Logger::debug(const char* message) { log(message, LogLevel::Debug); }
