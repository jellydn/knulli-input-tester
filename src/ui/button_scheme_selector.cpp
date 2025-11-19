#include "button_scheme_selector.h"
#include "../core/logger.h"

namespace knulli {
namespace ui {

ButtonSchemeSelectorWidget::ButtonSchemeSelectorWidget()
    : selectedScheme(input::ButtonScheme::Knulli),
      mapper(input::ButtonMapper::getInstance()) {
    core::Logger::getInstance().debug("ButtonSchemeSelectorWidget created");
}

ButtonSchemeSelectorWidget::~ButtonSchemeSelectorWidget() {
}

void ButtonSchemeSelectorWidget::render() {
    // Render button scheme selector using ImGui
    // ImGui::Begin("Button Scheme");
    // ImGui::RadioButton("KNULLI", ...);
    // ImGui::RadioButton("PlayStation", ...);
    // ImGui::RadioButton("Xbox", ...);
    // ImGui::End();
}

void ButtonSchemeSelectorWidget::update() {
    mapper.setActiveScheme(selectedScheme);
}

input::ButtonScheme ButtonSchemeSelectorWidget::getSelectedScheme() const {
    return selectedScheme;
}

void ButtonSchemeSelectorWidget::setSelectedScheme(input::ButtonScheme scheme) {
    selectedScheme = scheme;
    mapper.setActiveScheme(scheme);
}

} // namespace ui
} // namespace knulli
