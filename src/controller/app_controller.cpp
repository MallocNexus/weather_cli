#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include <mutex>

namespace weather_cli {

AppController::AppController(AppState& state, LocationController& location_controller, std::function<void()> on_quit)
    : state_(state), location_controller_(location_controller), on_quit_(on_quit) {}

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
    state_.is_loading = true;
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

void AppController::OpenSearch() {
    location_controller_.OpenSearch();
}

bool AppController::IsSearchModalOpen() const {
    std::lock_guard<std::mutex> lock(location_controller_.GetSearchState().mutex);
    return location_controller_.GetSearchState().show_search_modal;
}

void AppController::SelectPresetLocation(int index) {
    if (index == 1) {
        state_.latitude = -33.8688;
        state_.longitude = 151.2093;
        SearchCity("Sydney");
    } else if (index == 2) {
        state_.latitude = 40.7128;
        state_.longitude = -74.0060;
        SearchCity("New York");
    }
}

LocationController& AppController::GetLocationController() {
    return location_controller_;
}

const LocationController& AppController::GetLocationController() const {
    return location_controller_;
}

}  // namespace weather_cli
