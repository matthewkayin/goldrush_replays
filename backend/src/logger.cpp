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
        fflush(m_logfile);
    }
}

void Logger::logf(LogLevel level, const char* message, __builtin_va_list args) {
    const size_t MESSAGE_BUFFER_LENGTH = 32000;
    char out_message[MESSAGE_BUFFER_LENGTH];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_copy(arg_ptr, args);
    vsnprintf(out_message, MESSAGE_BUFFER_LENGTH, message, arg_ptr);
    va_end(arg_ptr);

    log(out_message, level);
}

void Logger::critical(const char* message, ...) {
    __builtin_va_list args;
    va_start(args, message);
    logf(LogLevel::Critical, message, args);
    va_end(args);
}

void Logger::error(const char* message, ...) {
    __builtin_va_list args;
    va_start(args, message);
    logf(LogLevel::Error, message, args);
    va_end(args);
}

void Logger::warn(const char* message, ...) {
    __builtin_va_list args;
    va_start(args, message);
    logf(LogLevel::Warning, message, args);
    va_end(args);
}

void Logger::info(const char* message, ...) {
    __builtin_va_list args;
    va_start(args, message);
    logf(LogLevel::Info, message, args);
    va_end(args);
}

void Logger::debug(const char* message, ...) {
    __builtin_va_list args;
    va_start(args, message);
    logf(LogLevel::Debug, message, args);
    va_end(args);
}

// Exception Format

exceptionf::exceptionf(const char* format, ...) {
    __builtin_va_list args;
    va_start(args, format);

    __builtin_va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (length > 0) {
        m_message.resize(length);
        vsnprintf(m_message.data(), m_message.size() + 1, format, args);
    } else {
        m_message = "Unknown formatted exception.";
    }

    va_end(args);
}

const char* exceptionf::what() const noexcept {
    return m_message.c_str();
}
