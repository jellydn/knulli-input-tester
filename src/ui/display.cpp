#include "display.h"
#include "../core/logger.h"
#include <memory>
#include <SDL2/SDL.h>

namespace knulli {
namespace ui {

// Concrete SDL2 Display implementation
class ImGuiDisplay : public Display {
public:
    ImGuiDisplay() 
        : initialized(false), window(nullptr), renderer(nullptr),
          width(800), height(600), refreshRate(60.0f), shouldCloseFlag(false) {
    }
    
    ~ImGuiDisplay() override {
        shutdown();
    }
    
    bool initialize() override {
        auto& logger = core::Logger::getInstance();
        logger.info("Initializing SDL2 Display");
        
        // Initialize SDL2
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            logger.error("SDL2 initialization failed: " + std::string(SDL_GetError()));
            return false;
        }
        
        // Create window
        window = SDL_CreateWindow(
            "Knulli Input Tester",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        
        if (!window) {
            logger.error("SDL2 window creation failed: " + std::string(SDL_GetError()));
            SDL_Quit();
            return false;
        }
        
        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            logger.error("SDL2 renderer creation failed: " + std::string(SDL_GetError()));
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        
        logger.info("SDL2 window and renderer created successfully");
        
        initialized = true;
        return true;
    }
    
    void shutdown() override {
        if (!initialized) {
            return;
        }
        
        auto& logger = core::Logger::getInstance();
        logger.info("Shutting down SDL2 Display");
        
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        
        SDL_Quit();
        initialized = false;
    }
    
    bool isInitialized() const override {
        return initialized && window != nullptr;
    }
    
    void beginFrame() override {
        if (!isInitialized()) {
            return;
        }
        
        // Process SDL2 events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    shouldCloseFlag = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        shouldCloseFlag = true;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        shouldCloseFlag = true;
                    }
                    break;
            }
        }
    }
    
    void endFrame() override {
        if (!isInitialized()) {
            return;
        }
        
        SDL_RenderPresent(renderer);
    }
    
    void clear() override {
        if (!isInitialized()) {
            return;
        }
        
        // Clear with dark background
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
    }
    
    void render() override {
        if (!isInitialized()) {
            return;
        }
        
        // Render title
        SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
        SDL_Rect title_rect = {10, 10, width - 20, 50};
        SDL_RenderFillRect(renderer, &title_rect);
        
        // Render status text (simple SDL2 text would need SDL_ttf)
        // For now just draw the rect
    }
    
    void setTitle(const std::string& title) override {
        if (window) {
            SDL_SetWindowTitle(window, title.c_str());
        }
    }
    
    void setSize(int w, int h) override {
        width = w;
        height = h;
        if (window) {
            SDL_SetWindowSize(window, w, h);
        }
    }
    
    int getWidth() const override {
        return width;
    }
    
    int getHeight() const override {
        return height;
    }
    
    bool shouldClose() const override {
        return shouldCloseFlag;
    }
    
    float getRefreshRate() const override {
        return refreshRate;
    }

private:
    bool initialized;
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    float refreshRate;
    bool shouldCloseFlag;
};

std::unique_ptr<Display> DisplayFactory::createDisplay(const std::string& type) {
    if (type == "imgui") {
        return std::make_unique<ImGuiDisplay>();
    }
    return nullptr;
}

} // namespace ui
} // namespace knulli
