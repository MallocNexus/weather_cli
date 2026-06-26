#include "controller/location_controller.hpp"
#include "controller/db_controller.hpp"

namespace weather_cli {

LocationController::LocationController(AppState& app_state, DatabaseController& db_controller, std::function<void()> trigger_redraw)
    : app_state_(app_state), db_controller_(db_controller), trigger_redraw_(trigger_redraw) {}

void LocationController::SelectLocation(const LocationMatch& location, bool save_to_db) {
    // Write results to main app state
    app_state_.city_name = location.name;
    app_state_.latitude = location.latitude;
    app_state_.longitude = location.longitude;

    if (save_to_db) {
        db_controller_.SaveLocation(location);
    }

    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

}  // namespace weather_cli
