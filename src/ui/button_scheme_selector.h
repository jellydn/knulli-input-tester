#ifndef KNULLI_UI_BUTTON_SCHEME_SELECTOR_H
#define KNULLI_UI_BUTTON_SCHEME_SELECTOR_H

#include "../input/button_mapper.h"

namespace knulli {
namespace ui {

class ButtonSchemeSelectorWidget {
public:
    ButtonSchemeSelectorWidget();
    ~ButtonSchemeSelectorWidget();
    
    void render();
    void update();
    
    input::ButtonScheme getSelectedScheme() const;
    void setSelectedScheme(input::ButtonScheme scheme);

private:
    input::ButtonScheme selectedScheme;
    input::ButtonMapper& mapper;
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_BUTTON_SCHEME_SELECTOR_H
