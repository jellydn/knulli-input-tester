#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "core/logger.h"
#include "core/config.h"
#include "core/service_integration.h"
#include "core/performance_monitor.h"
#include "input/device_manager.h"
#include "input/event_history.h"
#include "ui/display.h"
#include "ui/event_list.h"

using namespace knulli;

class InputTesterApplication {
public:
    InputTesterApplication() 
        : running(false), logger(core::Logger::getInstance()),
          config(core::Config::getInstance()),
          serviceIntegration(core::ServiceIntegration::getInstance()),
          performanceMonitor(core::PerformanceMonitor::getInstance()),
          inputManager(input::InputManager::getInstance()) {
    }
    
    ~InputTesterApplication() {
        shutdown();
    }
    
    bool initialize() {
        logger.initialize("knulli-input-tester");
        logger.info("=== Knulli Input Tester Starting ===");
        
        // Load configuration
        std::string configPath = core::Config::getDefaultConfigPath();
        config.load(configPath);
        logger.info("Configuration loaded");
        
        // Initialize service integration
        if (!serviceIntegration.initializeService("knulli-input-tester")) {
            logger.error("Failed to initialize service integration");
            return false;
        }
        
        // Detect input devices
        inputManager.detectDevices();
        size_t deviceCount = inputManager.getDeviceCount();
        logger.info("Detected " + std::to_string(deviceCount) + " input devices");
        
        // Initialize display
        display = ui::DisplayFactory::createDisplay("imgui");
        if (!display || !display->initialize()) {
            logger.error("Failed to initialize display");
            return false;
        }
        logger.info("Display initialized");
        
        // Initialize event history and UI widget
        eventHistory = std::make_unique<input::EventHistory>();
        eventListWidget = std::make_unique<ui::EventListWidget>();
        
        // Register event callback
        inputManager.registerEventCallback([this](const input::InputEvent& event) {
            onInputEvent(event);
        });
        
        // Start monitoring
        inputManager.startInputMonitoring();
        performanceMonitor.startMonitoring();
        
        running = true;
        logger.info("Application initialized successfully");
        return true;
    }
    
    void run() {
        logger.info("Starting main application loop");
        
        while (running && !display->shouldClose()) {
            display->beginFrame();
            
            // Process input events
            inputManager.processEvents();
            
            // Update UI
            if (eventHistory) {
                auto events = eventHistory->getEvents(20);
                eventListWidget->update(events);
            }
            
            // Render display
            display->clear();
            display->render();
            eventListWidget->render();
            display->endFrame();
            
            // Control frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
        }
        
        logger.info("Main application loop ended");
    }
    
    void shutdown() {
        if (!running) {
            return;
        }
        
        running = false;
        logger.info("Shutting down application");
        
        inputManager.stopInputMonitoring();
        performanceMonitor.stopMonitoring();
        
        if (display) {
            display->shutdown();
        }
        
        if (eventHistory) {
            eventHistory->clear();
        }
        
        serviceIntegration.shutdownService();
        logger.info("Application shutdown complete");
    }

private:
    void onInputEvent(const input::InputEvent& event) {
        // Add event to history
        if (eventHistory) {
            eventHistory->addEvent(event);
        }
        
        // Log event
        logger.debug("Input event: device=" + std::to_string(event.deviceId) + 
                   " type=" + event.description);
    }
    
    bool running;
    core::Logger& logger;
    core::Config& config;
    core::ServiceIntegration& serviceIntegration;
    core::PerformanceMonitor& performanceMonitor;
    input::InputManager& inputManager;
    
    std::unique_ptr<ui::Display> display;
    std::unique_ptr<input::EventHistory> eventHistory;
    std::unique_ptr<ui::EventListWidget> eventListWidget;
};

int main(int argc, char* argv[]) {
    try {
        InputTesterApplication app;
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }
        
        app.run();
        
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 2;
    }
}
