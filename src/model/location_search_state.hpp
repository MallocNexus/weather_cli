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
    std::vector<LocationMatch> search_suggestions;
    int selected_suggestion_index = 0;
    bool is_loading = false;
    bool has_error = false;
    std::string error_message = "";
    
    // Mutex to protect shared search state between TUI thread and background worker thread
    mutable std::mutex mutex;
};

}  // namespace weather_cli
