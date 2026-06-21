#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "controller/about_controller.hpp"

namespace weather_cli {

class AboutView {
public:
    AboutView(AboutController& controller);

    // Returns the TUI component representing the modal contents
    ftxui::Component GetComponent();

    // Renders the modal element as an overlay layer on top of a base document element
    ftxui::Element Render(ftxui::Element base_document);

private:
    AboutController& controller_;
    ftxui::Component close_button_;
    ftxui::Component view_component_;
};

}  // namespace weather_cli
