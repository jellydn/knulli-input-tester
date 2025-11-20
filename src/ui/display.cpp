#include "display.h"
#include "../core/logger.h"
#include <memory>

namespace knulli {
namespace ui {

// Concrete ImGui Display implementation
class ImGuiDisplay : public Display {
public:
    ImGuiDisplay() : initialized(false), width(800), height(600), refreshRate(60.0f) {
    }
    
    ~ImGuiDisplay() override {
        shutdown();
    }
    
    bool initialize() override {
        core::Logger::getInstance().info("Initializing ImGui Display");
        // SDL2 + ImGui initialization would go here
        initialized = true;
        return true;
    }
    
    void shutdown() override {
        if (initialized) {
            core::Logger::getInstance().info("Shutting down ImGui Display");
            initialized = false;
        }
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    void beginFrame() override {
        // Begin ImGui frame
    }
    
    void endFrame() override {
        // End ImGui frame and present
    }
    
    void clear() override {
        // Clear screen
    }
    
    void render() override {
        // Render ImGui content
    }
    
    void setTitle(const std::string& /* title */) override {
        // Set window title
    }
    
    void setSize(int w, int h) override {
        width = w;
        height = h;
    }
    
    int getWidth() const override {
        return width;
    }
    
    int getHeight() const override {
        return height;
    }
    
    bool shouldClose() const override {
        return false;  // Check actual SDL2 window close flag
    }
    
    float getRefreshRate() const override {
        return refreshRate;
    }

private:
    bool initialized;
    int width;
    int height;
    float refreshRate;
};

std::unique_ptr<Display> DisplayFactory::createDisplay(const std::string& type) {
    if (type == "imgui") {
        return std::make_unique<ImGuiDisplay>();
    }
    return nullptr;
}

} // namespace ui
} // namespace knulli
