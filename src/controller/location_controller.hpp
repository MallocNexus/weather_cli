#pragma once

#include "model/app_state.hpp"
#include "service/geocoding_service.hpp" // for LocationMatch
#include <functional>
#include <string>

namespace weather_cli {

class DatabaseController;

class LocationController {
public:
    LocationController(AppState& app_state, DatabaseController& db_controller, std::function<void()> trigger_redraw);

    // Called when a location is selected.
    // Updates AppState with coordinate decisions and delegates SQLite save.
    void SelectLocation(const LocationMatch& location, bool save_to_db);

private:
    AppState& app_state_;
    DatabaseController& db_controller_;
    std::function<void()> trigger_redraw_;
};

}  // namespace weather_cli
