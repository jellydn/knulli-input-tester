#include "logger.h"
#include <syslog.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace knulli {
namespace core {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() 
    : currentLogLevel(LogLevel::INFO), syslogInitialized(false) {
}

Logger::~Logger() {
    shutdown();
}

void Logger::initialize(const std::string& appName) {
    if (!syslogInitialized) {
        openlog(appName.c_str(), LOG_PID | LOG_CONS, LOG_USER);
        syslogInitialized = true;
        info("Logger initialized for " + appName);
    }
}

void Logger::shutdown() {
    if (syslogInitialized) {
        closelog();
        syslogInitialized = false;
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLogLevel) {
        return;
    }
    
    // Convert to syslog priority
    int priority = LOG_INFO;
    std::string levelStr = "INFO";
    
    switch (level) {
        case LogLevel::DEBUG:
            priority = LOG_DEBUG;
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            priority = LOG_INFO;
            levelStr = "INFO";
            break;
        case LogLevel::WARNING:
            priority = LOG_WARNING;
            levelStr = "WARNING";
            break;
        case LogLevel::ERROR:
            priority = LOG_ERR;
            levelStr = "ERROR";
            break;
        case LogLevel::CRITICAL:
            priority = LOG_CRIT;
            levelStr = "CRITICAL";
            break;
    }
    
    // Get current timestamp
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    
    std::string logMessage = "[" + oss.str() + "] [" + levelStr + "] " + message;
    
    // Log to syslog if initialized, otherwise to stderr
    if (syslogInitialized) {
        syslog(priority, "%s", logMessage.c_str());
    } else {
        std::cerr << logMessage << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

LogLevel Logger::getLogLevel() const {
    return currentLogLevel;
}

} // namespace core
} // namespace knulli
