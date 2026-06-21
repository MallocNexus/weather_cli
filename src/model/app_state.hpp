#pragma once

#include <string>
#include <vector>

namespace weather_cli {

enum class ViewTab {
    kHourlyGraph = 0,
    kRainGraph = 1
};

struct LocationMatch {
    std::string name = "";
    std::string country = "";
    std::string region = "";
    double latitude = 0.0;
    double longitude = 0.0;
};

struct AppState {
    // Current location coordinates and name
    double latitude = -33.8688;
    double longitude = 151.2093;
    std::string city_name = "Sydney";

    // Application state status
    bool is_loading = false;
    bool has_error = false;
    std::string error_message = "";

    // Toggle unit configurations (true = Celsius, false = Fahrenheit)
    bool is_celsius = true;

    // View tab selections (Temperature Graph vs Rain Probability Graph)
    ViewTab active_tab = ViewTab::kHourlyGraph;

    // Timeline timeline index
    int selected_hour_index = 9; // Default to 9:00 AM (0 to 23 range)

    // Location search state
    bool show_search_modal = false;
    std::string search_query = "";
    std::vector<LocationMatch> search_suggestions;
    int selected_suggestion_index = 0;
};


}  // namespace weather_cli
