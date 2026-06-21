#pragma once

#include <ftxui/component/component.hpp>
#include <string>
#include <vector>

#include "controller/app_controller.hpp"
#include "model/app_state.hpp"
#include "view/about_view.hpp"
#include "view/location_search_view.hpp"

namespace weather_cli {

class App {
   public:
    App(AppState& state, AppController& controller);
    ftxui::Component GetComponent();

   private:
    AppState& state_;
    AppController& controller_;
    LocationSearchView location_search_view_;
    AboutView about_view_;
    ftxui::Component main_renderer_;

    // Menu configurations
    std::vector<std::string> top_menu_entries_ = {"File", "Locations", "Units", "Help"};
    int top_menu_selected_ = 0;

    std::vector<std::string> app_entries_ = {"Refresh", "Quit"};
    int app_selected_ = 0;

    std::vector<std::string> locations_entries_ = {"Search Location", "Sydney, AU", "New York, US"};
    int locations_selected_ = 0;

    std::vector<std::string> units_entries_ = {"Celsius (°C)", "Fahrenheit (°F)"};
    int units_selected_ = 0;

    std::vector<std::string> help_entries_ = {"About", "Shortcuts"};
    int help_selected_ = 0;

    // View tab selection (Temperature vs Rain graph tab selectors)
    std::vector<std::string> graph_tab_entries_ = {"Hourly Temperature Graph",
                                                   "Hourly Rain Probability"};
    int graph_tab_selected_ = 0;

    // Root layout selector tab index
    int root_tab_selected_ = 0;
};

}  // namespace weather_cli
