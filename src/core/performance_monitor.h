#ifndef KNULLI_CORE_PERFORMANCE_MONITOR_H
#define KNULLI_CORE_PERFORMANCE_MONITOR_H

#include <chrono>
#include <string>
#include <map>
#include <memory>

namespace knulli {
namespace core {

struct PerformanceMetrics {
    float cpuUsagePercent;
    size_t memoryUsageBytes;
    float averageLatencyMs;
    int eventsProcessed;
    float fpsDrop;
};

class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Metrics collection
    void recordEventLatency(std::chrono::milliseconds latency);
    void recordMemoryUsage(size_t bytes);
    void recordEventCount(int count);
    
    // Metrics retrieval
    PerformanceMetrics getMetrics();
    float getAverageCPUUsage();
    float getAverageMemoryUsage();
    float getAverageLatency();
    
    // Performance alerts
    bool isWithinPerformanceBudget();
    void setMaxCPUUsage(float maxPercent);
    void setMaxMemoryUsage(size_t maxBytes);
    void setMaxLatency(std::chrono::milliseconds maxLatency);

private:
    PerformanceMonitor();
    ~PerformanceMonitor();
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    bool monitoring;
    std::chrono::steady_clock::time_point startTime;
    float maxCPUUsagePercent;
    size_t maxMemoryUsageBytes;
    std::chrono::milliseconds maxLatencyMs;
    
    // Performance history
    std::map<std::chrono::steady_clock::time_point, float> latencyHistory;
    std::map<std::chrono::steady_clock::time_point, size_t> memoryHistory;
    int totalEventsProcessed;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_PERFORMANCE_MONITOR_H
