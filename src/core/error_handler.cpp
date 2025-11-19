#include "error_handler.h"
#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <exception>
#include <map>

namespace knulli {
namespace core {

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() {
}

ErrorHandler::~ErrorHandler() {
}

void ErrorHandler::handleError(ErrorCode code, const std::string& message, const std::string& context) {
    ErrorInfo error;
    error.code = code;
    error.message = message;
    error.context = context;
    
    // Get timestamp
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    error.timestamp = oss.str();
    
    logError(error);
    
    // Try error recovery
    if (recoveryCallback) {
        if (!recoverFromError(code)) {
            Logger::getInstance().critical("Failed to recover from error: " + message);
        }
    }
    
    // Track error count
    errorCounts[code]++;
}

void ErrorHandler::handleException(const std::exception& ex) {
    Logger::getInstance().critical("Unhandled exception: " + std::string(ex.what()));
    handleError(ErrorCode::UNKNOWN, std::string(ex.what()));
}

bool ErrorHandler::recoverFromError(ErrorCode code) {
    if (!recoveryCallback) {
        return false;
    }
    
    switch (code) {
        case ErrorCode::DEVICE_NOT_FOUND:
            Logger::getInstance().info("Attempting to recover from DEVICE_NOT_FOUND");
            return recoveryCallback(code);
            
        case ErrorCode::PERMISSION_DENIED:
            Logger::getInstance().warning("PERMISSION_DENIED - manual intervention may be required");
            return false;
            
        case ErrorCode::TIMEOUT:
            Logger::getInstance().info("Attempting to recover from TIMEOUT");
            return recoveryCallback(code);
            
        default:
            return recoveryCallback(code);
    }
}

void ErrorHandler::setErrorRecoveryCallback(std::function<bool(ErrorCode)> callback) {
    recoveryCallback = callback;
}

void ErrorHandler::logError(const ErrorInfo& error) {
    std::string errorCodeStr;
    switch (error.code) {
        case ErrorCode::SUCCESS: errorCodeStr = "SUCCESS"; break;
        case ErrorCode::DEVICE_NOT_FOUND: errorCodeStr = "DEVICE_NOT_FOUND"; break;
        case ErrorCode::DEVICE_ERROR: errorCodeStr = "DEVICE_ERROR"; break;
        case ErrorCode::PERMISSION_DENIED: errorCodeStr = "PERMISSION_DENIED"; break;
        case ErrorCode::CONFIG_ERROR: errorCodeStr = "CONFIG_ERROR"; break;
        case ErrorCode::MEMORY_ERROR: errorCodeStr = "MEMORY_ERROR"; break;
        case ErrorCode::THREAD_ERROR: errorCodeStr = "THREAD_ERROR"; break;
        case ErrorCode::TIMEOUT: errorCodeStr = "TIMEOUT"; break;
        case ErrorCode::UNKNOWN: errorCodeStr = "UNKNOWN"; break;
    }
    
    std::string logMessage = "[" + error.timestamp + "] ERROR[" + errorCodeStr + "]: " + error.message;
    if (!error.context.empty()) {
        logMessage += " (context: " + error.context + ")";
    }
    
    Logger::getInstance().error(logMessage);
}

void ErrorHandler::logWarning(const std::string& message) {
    Logger::getInstance().warning(message);
}

int ErrorHandler::getErrorCount(ErrorCode code) {
    auto it = errorCounts.find(code);
    if (it != errorCounts.end()) {
        return it->second;
    }
    return 0;
}

int ErrorHandler::getTotalErrorCount() {
    int total = 0;
    for (const auto& [code, count] : errorCounts) {
        total += count;
    }
    return total;
}

void ErrorHandler::resetErrorStats() {
    errorCounts.clear();
}

} // namespace core
} // namespace knulli
