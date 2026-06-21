#pragma once

#include "model/app_state.hpp"
#include <functional>
#include <string>

namespace weather_cli {

class AppController {
public:
    AppController(AppState& state, std::function<void()> on_quit);

    // Coordinate actions triggered by the TUI View
    void ToggleUnits();
    void SetActiveTab(ViewTab tab);
    void UpdateSliderIndex(int index);
    void RefreshForecast();
    void SearchCity(const std::string& query);
    void Quit();

private:
    AppState& state_;
    std::function<void()> on_quit_;
};

}  // namespace weather_cli
