#include "button_mapper.h"
#include "../core/logger.h"

namespace knulli {
namespace input {

ButtonMapper& ButtonMapper::getInstance() {
    static ButtonMapper instance;
    return instance;
}

ButtonMapper::ButtonMapper() : activeScheme(ButtonScheme::Knulli) {
    initializeMappings();
    core::Logger::getInstance().info("ButtonMapper initialized");
}

ButtonMapper::~ButtonMapper() {
}

void ButtonMapper::initializeMappings() {
    // Map all button codes to their labels in different schemes
    mappings[InputCode::ButtonA] = {
        InputCode::ButtonA,
        "○",           // PlayStation circle
        "A",           // Xbox A
        "A Button (East)"
    };
    
    mappings[InputCode::ButtonB] = {
        InputCode::ButtonB,
        "✕",           // PlayStation cross
        "B",           // Xbox B
        "B Button (South)"
    };
    
    mappings[InputCode::ButtonX] = {
        InputCode::ButtonX,
        "△",           // PlayStation triangle
        "X",           // Xbox X
        "X Button (North)"
    };
    
    mappings[InputCode::ButtonY] = {
        InputCode::ButtonY,
        "◻",           // PlayStation square
        "Y",           // Xbox Y
        "Y Button (West)"
    };
    
    // Add more mappings as needed...
}

ButtonMapping ButtonMapper::getMapping(InputCode code) const {
    auto it = mappings.find(code);
    if (it != mappings.end()) {
        return it->second;
    }
    return ButtonMapping{code, "?", "?", "Unknown"};
}

std::string ButtonMapper::getLabel(InputCode code, ButtonScheme scheme) const {
    auto mapping = getMapping(code);
    
    switch (scheme) {
        case ButtonScheme::Knulli:
            switch (code) {
                case InputCode::ButtonA: return "A";
                case InputCode::ButtonB: return "B";
                case InputCode::ButtonX: return "X";
                case InputCode::ButtonY: return "Y";
                default: return mapping.description;
            }
        case ButtonScheme::PlayStation:
            return mapping.playstationLabel;
        case ButtonScheme::Xbox:
            return mapping.xboxLabel;
        default:
            return "?";
    }
}

void ButtonMapper::setActiveScheme(ButtonScheme scheme) {
    activeScheme = scheme;
    std::string schemeName = (scheme == ButtonScheme::Knulli ? "Knulli" : 
                               scheme == ButtonScheme::PlayStation ? "PlayStation" : "Xbox");
    core::Logger::getInstance().info("Button scheme changed to: " + schemeName);
}

ButtonScheme ButtonMapper::getActiveScheme() const {
    return activeScheme;
}

InputCode ButtonMapper::translateCode(InputCode code, ButtonScheme /*from*/, ButtonScheme /*to*/) const {
    // Code identity doesn't change, just labels change
    return code;
}

std::string ButtonMapper::translateLabel(const std::string& label, ButtonScheme from, ButtonScheme to) const {
    // Find the input code first
    for (const auto& [code, mapping] : mappings) {
        std::string fromLabel;
        
        switch (from) {
            case ButtonScheme::Knulli: 
                if (code == InputCode::ButtonA) fromLabel = "A";
                else if (code == InputCode::ButtonB) fromLabel = "B";
                else if (code == InputCode::ButtonX) fromLabel = "X";
                else if (code == InputCode::ButtonY) fromLabel = "Y";
                break;
            case ButtonScheme::PlayStation:
                fromLabel = mapping.playstationLabel;
                break;
            case ButtonScheme::Xbox:
                fromLabel = mapping.xboxLabel;
                break;
        }
        
        if (fromLabel == label) {
            return getLabel(code, to);
        }
    }
    
    return label;  // Unknown label, return as-is
}

} // namespace input
} // namespace knulli
