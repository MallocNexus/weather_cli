#pragma once

#include "model/app_state.hpp"
#include <functional>
#include <string>

namespace weather_cli {

class LocationController;
class AboutController;
class DatabaseController;

class AppController {
public:
    AppController(AppState& state, LocationController& location_controller, AboutController& about_controller, DatabaseController& db_controller, std::function<void()> on_quit);

    // Coordinate actions triggered by the TUI View
    void ToggleUnits();
    void SetActiveTab(ViewTab tab);
    void UpdateSliderIndex(int index);
    void RefreshForecast();
    void SearchCity(const std::string& query);
    void Quit();

    // Hierarchical MVC additions
    void OpenSearch();
    bool IsSearchModalOpen() const;
    void SelectPresetLocation(int index);

    void OpenAbout();
    bool IsAboutModalOpen() const;

    void SelectSavedLocation(int saved_index);

    LocationController& GetLocationController();
    const LocationController& GetLocationController() const;

    AboutController& GetAboutController();
    const AboutController& GetAboutController() const;

    DatabaseController& GetDatabaseController();
    const DatabaseController& GetDatabaseController() const;

private:
    AppState& state_;
    LocationController& location_controller_;
    AboutController& about_controller_;
    DatabaseController& db_controller_;
    std::function<void()> on_quit_;
};

}  // namespace weather_cli
