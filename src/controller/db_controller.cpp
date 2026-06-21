#include "controller/db_controller.hpp"

namespace weather_cli {

DatabaseController::DatabaseController(LocationRepository& repo, std::function<void()> trigger_redraw)
    : repo_(repo), trigger_redraw_(trigger_redraw) {}

bool DatabaseController::InitializeDatabase() {
    return repo_.Initialize();
}

bool DatabaseController::SaveLocation(const LocationMatch& location) {
    bool success = repo_.SaveLocation(location);
    if (success && trigger_redraw_) {
        trigger_redraw_();
    }
    return success;
}

bool DatabaseController::LoadSavedLocations() {
    bool success = repo_.LoadSavedLocations();
    if (success && trigger_redraw_) {
        trigger_redraw_();
    }
    return success;
}

}  // namespace weather_cli
