#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

#include "controller/location_controller.hpp"

namespace weather_cli {

class LocationSearchView {
public:
    LocationSearchView(LocationController& controller);
    
    // Returns the main interactive TUI component representing the modal contents
    ftxui::Component GetComponent();

    // Renders the modal element as an overlay layer on top of a base document element
    ftxui::Element Render(ftxui::Element base_document);

private:
    LocationController& controller_;
    std::vector<std::string> suggestion_entries_;
    ftxui::Component search_input_;
    ftxui::Component suggestions_menu_;
    ftxui::Component save_checkbox_;
    ftxui::Component view_component_;
    bool last_show_search_modal_ = false;
};

}  // namespace weather_cli
