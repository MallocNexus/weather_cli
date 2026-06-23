#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "controller/about_controller.hpp"
#include "controller/db_controller.hpp"
#include "controller/forecast_controller.hpp"
#include <mutex>

namespace weather_cli {

AppController::AppController(AppState& state, LocationController& location_controller, AboutController& about_controller, DatabaseController& db_controller, ForecastController& forecast_controller, std::function<void()> on_quit)
    : state_(state), location_controller_(location_controller), about_controller_(about_controller), db_controller_(db_controller), forecast_controller_(forecast_controller), on_quit_(on_quit) {}

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
    forecast_controller_.Refresh();
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

void AppController::OpenAbout() {
    about_controller_.OpenAbout();
}

bool AppController::IsAboutModalOpen() const {
    return about_controller_.GetAboutState().show_about_modal;
}

AboutController& AppController::GetAboutController() {
    return about_controller_;
}

const AboutController& AppController::GetAboutController() const {
    return about_controller_;
}

void AppController::SelectSavedLocation(int saved_index) {
    const auto& repo = db_controller_.GetRepository();
    const auto& saved_locs = repo.GetSavedLocations();
    if (saved_index >= 0 && saved_index < static_cast<int>(saved_locs.size())) {
        const auto& loc = saved_locs[saved_index];
        state_.latitude = loc.latitude;
        state_.longitude = loc.longitude;
        SearchCity(loc.name);
    }
}

DatabaseController& AppController::GetDatabaseController() {
    return db_controller_;
}

const DatabaseController& AppController::GetDatabaseController() const {
    return db_controller_;
}

ForecastController& AppController::GetForecastController() {
    return forecast_controller_;
}

const ForecastController& AppController::GetForecastController() const {
    return forecast_controller_;
}

}  // namespace weather_cli
