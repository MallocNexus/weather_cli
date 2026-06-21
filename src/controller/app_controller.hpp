#pragma once

#include "model/app_state.hpp"
#include <functional>
#include <string>

namespace weather_cli {

class LocationController;

class AppController {
public:
    AppController(AppState& state, LocationController& location_controller, std::function<void()> on_quit);

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

    LocationController& GetLocationController();
    const LocationController& GetLocationController() const;

private:
    AppState& state_;
    LocationController& location_controller_;
    std::function<void()> on_quit_;
};

}  // namespace weather_cli
