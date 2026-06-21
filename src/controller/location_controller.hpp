#pragma once

#include "model/app_state.hpp"
#include <functional>
#include <string>

namespace weather_cli {

class LocationController {
public:
    LocationController(AppState& state, std::function<void()> trigger_redraw);

    // Invokes background thread GeocodingService calls
    void Search(const std::string& query);

    // Selects suggestion and updates coordinates state
    void SelectSuggestion(int index);

private:
    AppState& state_;
    std::function<void()> trigger_redraw_;
};

}  // namespace weather_cli
