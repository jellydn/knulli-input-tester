#ifndef KNULLI_INPUT_BUTTON_MAPPER_H
#define KNULLI_INPUT_BUTTON_MAPPER_H

#include "input_event.h"
#include <string>
#include <map>

namespace knulli {
namespace input {

enum class ButtonScheme {
    Knulli,         // Default KNULLI labeling (A/B/X/Y cardinal)
    PlayStation,    // △/○/✕/◻ symbols
    Xbox            // A/B/X/Y labels
};

struct ButtonMapping {
    InputCode knulliCode;
    std::string playstationLabel;
    std::string xboxLabel;
    std::string description;
};

class ButtonMapper {
public:
    static ButtonMapper& getInstance();
    
    // Get mappings
    ButtonMapping getMapping(InputCode code) const;
    std::string getLabel(InputCode code, ButtonScheme scheme) const;
    
    // Scheme management
    void setActiveScheme(ButtonScheme scheme);
    ButtonScheme getActiveScheme() const;
    
    // Translation
    InputCode translateCode(InputCode code, ButtonScheme from, ButtonScheme to) const;
    std::string translateLabel(const std::string& label, ButtonScheme from, ButtonScheme to) const;

private:
    ButtonMapper();
    ~ButtonMapper();
    ButtonMapper(const ButtonMapper&) = delete;
    ButtonMapper& operator=(const ButtonMapper&) = delete;
    
    void initializeMappings();
    std::map<InputCode, ButtonMapping> mappings;
    ButtonScheme activeScheme;
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_BUTTON_MAPPER_H
