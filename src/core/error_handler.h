#ifndef KNULLI_CORE_ERROR_HANDLER_H
#define KNULLI_CORE_ERROR_HANDLER_H

#include <string>
#include <functional>
#include <memory>
#include <map>

namespace knulli {
namespace core {

enum class ErrorCode {
    SUCCESS = 0,
    DEVICE_NOT_FOUND = 1,
    DEVICE_ERROR = 2,
    PERMISSION_DENIED = 3,
    CONFIG_ERROR = 4,
    MEMORY_ERROR = 5,
    THREAD_ERROR = 6,
    TIMEOUT = 7,
    UNKNOWN = 99
};

struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string context;
    std::string timestamp;
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    // Error handling
    void handleError(ErrorCode code, const std::string& message, const std::string& context = "");
    void handleException(const std::exception& ex);
    
    // Error recovery
    bool recoverFromError(ErrorCode code);
    void setErrorRecoveryCallback(std::function<bool(ErrorCode)> callback);
    
    // Error logging
    void logError(const ErrorInfo& error);
    void logWarning(const std::string& message);
    
    // Error statistics
    int getErrorCount(ErrorCode code);
    int getTotalErrorCount();
    void resetErrorStats();

private:
    ErrorHandler();
    ~ErrorHandler();
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    std::function<bool(ErrorCode)> recoveryCallback;
    std::map<ErrorCode, int> errorCounts;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_ERROR_HANDLER_H
