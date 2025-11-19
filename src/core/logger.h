#ifndef KNULLI_CORE_LOGGER_H
#define KNULLI_CORE_LOGGER_H

#include <string>
#include <memory>

namespace knulli {
namespace core {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
public:
    static Logger& getInstance();
    
    void initialize(const std::string& appName);
    void shutdown();
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    LogLevel currentLogLevel;
    bool syslogInitialized;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_LOGGER_H
