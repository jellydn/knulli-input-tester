#ifndef KNULLI_UI_DISPLAY_H
#define KNULLI_UI_DISPLAY_H

#include <string>
#include <memory>
#include <vector>

namespace knulli {
namespace ui {

class Display {
public:
    virtual ~Display() = default;
    
    // Display lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Rendering
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear() = 0;
    virtual void render() = 0;
    
    // Window management
    virtual void setTitle(const std::string& title) = 0;
    virtual void setSize(int width, int height) = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    
    // Display properties
    virtual bool shouldClose() const = 0;
    virtual float getRefreshRate() const = 0;
};

class DisplayFactory {
public:
    static std::unique_ptr<Display> createDisplay(const std::string& type = "imgui");
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_DISPLAY_H
