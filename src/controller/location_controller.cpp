#include "controller/location_controller.hpp"
#include "service/geocoding_service.hpp"
#include <thread>

namespace weather_cli {

LocationController::LocationController(AppState& state, std::function<void()> trigger_redraw)
    : state_(state), trigger_redraw_(trigger_redraw) {}

void LocationController::Search(const std::string& query) {
    state_.search_query = query;
    state_.is_loading = true;
    state_.has_error = false;

    // Dispatch geocoding query onto background worker thread
    std::thread([this, query]() {
        try {
            auto matches = GeocodingService::Search(query);

            state_.search_suggestions = matches;
            state_.selected_suggestion_index = 0;
            state_.is_loading = false;

            if (trigger_redraw_) {
                trigger_redraw_();
            }
        } catch (const std::exception& e) {
            state_.is_loading = false;
            state_.has_error = true;
            state_.error_message = e.what();
            if (trigger_redraw_) {
                trigger_redraw_();
            }
        }
    }).detach();
}

void LocationController::SelectSuggestion(int index) {
    if (index >= 0 && index < static_cast<int>(state_.search_suggestions.size())) {
        const auto& match = state_.search_suggestions[index];
        state_.city_name = match.name;
        state_.latitude = match.latitude;
        state_.longitude = match.longitude;

        // Reset query modal parameters
        state_.show_search_modal = false;
        state_.search_query = "";
        state_.search_suggestions.clear();
        state_.selected_suggestion_index = 0;

        if (trigger_redraw_) {
            trigger_redraw_();
        }
    }
}

}  // namespace weather_cli
