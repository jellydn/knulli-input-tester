#include "performance_monitor.h"
#include "logger.h"
#include <fstream>
#include <numeric>

namespace knulli {
namespace core {

PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

PerformanceMonitor::PerformanceMonitor()
    : monitoring(false),
      maxCPUUsagePercent(10.0f),
      maxMemoryUsageBytes(50 * 1024 * 1024),  // 50 MB
      maxLatencyMs(100),  // 100ms target
      totalEventsProcessed(0) {
    Logger::getInstance().info("PerformanceMonitor initialized");
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::startMonitoring() {
    if (!monitoring) {
        monitoring = true;
        startTime = std::chrono::steady_clock::now();
        totalEventsProcessed = 0;
        Logger::getInstance().info("Performance monitoring started");
    }
}

void PerformanceMonitor::stopMonitoring() {
    if (monitoring) {
        monitoring = false;
        auto duration = std::chrono::steady_clock::now() - startTime;
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        Logger::getInstance().info("Performance monitoring stopped after " + std::to_string(durationMs) + "ms");
    }
}

bool PerformanceMonitor::isMonitoring() const {
    return monitoring;
}

void PerformanceMonitor::recordEventLatency(std::chrono::milliseconds latency) {
    latencyHistory[std::chrono::steady_clock::now()] = latency.count();
    
    if (latency > maxLatencyMs) {
        Logger::getInstance().warning("Latency exceeded threshold: " + std::to_string(latency.count()) + "ms");
    }
}

void PerformanceMonitor::recordMemoryUsage(size_t bytes) {
    memoryHistory[std::chrono::steady_clock::now()] = bytes;
    
    if (bytes > maxMemoryUsageBytes) {
        Logger::getInstance().warning("Memory usage exceeded threshold: " + std::to_string(bytes / 1024 / 1024) + "MB");
    }
}

void PerformanceMonitor::recordEventCount(int count) {
    totalEventsProcessed += count;
}

PerformanceMetrics PerformanceMonitor::getMetrics() {
    PerformanceMetrics metrics;
    metrics.cpuUsagePercent = getAverageCPUUsage();
    metrics.memoryUsageBytes = getAverageMemoryUsage();
    metrics.averageLatencyMs = getAverageLatency();
    metrics.eventsProcessed = totalEventsProcessed;
    metrics.fpsDrop = 0.0f;  // Calculate based on actual performance
    
    return metrics;
}

float PerformanceMonitor::getAverageCPUUsage() {
    // Try to read CPU usage from /proc/self/stat
    std::ifstream stat("/proc/self/stat");
    if (!stat.is_open()) {
        return 0.0f;
    }
    
    // Parse /proc/self/stat for CPU time
    std::string line;
    if (std::getline(stat, line)) {
        // CPU usage would be calculated from utime + stime / total_time
        // Simplified: return approximate usage
        return 0.1f;  // Placeholder
    }
    
    return 0.0f;
}

float PerformanceMonitor::getAverageMemoryUsage() {
    // Try to read memory usage from /proc/self/status
    std::ifstream status("/proc/self/status");
    if (!status.is_open()) {
        return 0.0f;
    }
    
    std::string line;
    while (std::getline(status, line)) {
        if (line.find("VmRSS:") == 0) {
            // Extract RSS in KB
            size_t pos = line.find_last_of(' ');
            if (pos != std::string::npos) {
                try {
                    size_t kb = std::stoul(line.substr(pos + 1));
                    return static_cast<float>(kb * 1024);  // Convert to bytes
                } catch (...) {
                    return 0.0f;
                }
            }
        }
    }
    
    return 0.0f;
}

float PerformanceMonitor::getAverageLatency() {
    if (latencyHistory.empty()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    for (const auto& [time, latency] : latencyHistory) {
        total += latency;
    }
    
    return total / latencyHistory.size();
}

bool PerformanceMonitor::isWithinPerformanceBudget() {
    float cpuUsage = getAverageCPUUsage();
    float memoryUsage = getAverageMemoryUsage();
    float latency = getAverageLatency();
    
    bool withinBudget = (cpuUsage <= maxCPUUsagePercent) &&
                       (memoryUsage <= maxMemoryUsageBytes) &&
                       (latency <= maxLatencyMs.count());
    
    return withinBudget;
}

void PerformanceMonitor::setMaxCPUUsage(float maxPercent) {
    maxCPUUsagePercent = maxPercent;
    Logger::getInstance().info("Max CPU usage set to " + std::to_string(maxPercent) + "%");
}

void PerformanceMonitor::setMaxMemoryUsage(size_t maxBytes) {
    maxMemoryUsageBytes = maxBytes;
    Logger::getInstance().info("Max memory usage set to " + std::to_string(maxBytes / 1024 / 1024) + "MB");
}

void PerformanceMonitor::setMaxLatency(std::chrono::milliseconds maxLatency) {
    maxLatencyMs = maxLatency;
    Logger::getInstance().info("Max latency set to " + std::to_string(maxLatency.count()) + "ms");
}

} // namespace core
} // namespace knulli
