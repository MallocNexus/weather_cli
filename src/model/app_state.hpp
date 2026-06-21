#pragma once

#include <string>
#include <vector>

namespace weather_cli {

enum class ViewTab {
    kHourlyGraph = 0,
    kRainGraph = 1
};

struct AppState {
    // Current location coordinates and name
    double latitude = -33.8688;
    double longitude = 151.2093;
    std::string city_name = "Sydney";

    // Application state status (for forecast loading/errors)
    bool is_loading = false;
    bool has_error = false;
    std::string error_message = "";

    // Toggle unit configurations (true = Celsius, false = Fahrenheit)
    bool is_celsius = true;

    // View tab selections (Temperature Graph vs Rain Probability Graph)
    ViewTab active_tab = ViewTab::kHourlyGraph;

    // Timeline timeline index
    int selected_hour_index = 9; // Default to 9:00 AM (0 to 23 range)
};


}  // namespace weather_cli
