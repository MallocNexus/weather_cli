#include "controller/app_controller.hpp"

namespace weather_cli {

AppController::AppController(AppState& state, std::function<void()> on_quit)
    : state_(state), on_quit_(on_quit) {}

void AppController::ToggleUnits() {
    state_.is_celsius = !state_.is_celsius;
}

void AppController::SetActiveTab(ViewTab tab) {
    state_.active_tab = tab;
}

void AppController::UpdateSliderIndex(int index) {
    state_.selected_hour_index = index;
}

void AppController::RefreshForecast() {
    // In Phase 3, refresh triggers a static loading state toggling to check loading frames
    state_.is_loading = true;
    // Just toggle loading status off since we have no network backend yet
    state_.is_loading = false;
}

void AppController::SearchCity(const std::string& query) {
    state_.city_name = query;
}

void AppController::Quit() {
    if (on_quit_) {
        on_quit_();
    }
}

}  // namespace weather_cli
