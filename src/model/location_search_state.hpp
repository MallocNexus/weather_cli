#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace weather_cli {

struct LocationMatch {
    std::string name = "";
    std::string country = "";
    std::string region = "";
    double latitude = 0.0;
    double longitude = 0.0;
};

struct LocationSearchState {
    bool show_search_modal = false;
    bool save_to_db = false;
    std::string search_query = "";
    int cursor_position = 0;
    std::vector<LocationMatch> search_suggestions;
    int selected_suggestion_index = 0;
    bool is_loading = false;
    bool has_error = false;
    std::string error_message = "";

    // Country filter applied to GeocodingService::Search queries.
    // Stores the ISO 3166-1 alpha-2 code (e.g. "AU", "US"). Empty string means
    // no country filter. Defaults to Australia so searches are scoped locally.
    std::string country_filter = "AU";

    // Index into kCountryList that drives the dropdown selection in the view.
    // Must stay in sync with country_filter at all times.
    int country_filter_index = 0;

    // Mutex to protect shared search state between TUI thread and background worker thread
    mutable std::mutex mutex;
};

}  // namespace weather_cli
