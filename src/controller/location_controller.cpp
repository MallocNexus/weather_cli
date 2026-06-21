#include "controller/location_controller.hpp"
#include "controller/db_controller.hpp"
#include "service/geocoding_service.hpp"
#include <thread>
#include <mutex>

namespace weather_cli {

LocationController::LocationController(AppState& app_state, DatabaseController& db_controller, std::function<void()> trigger_redraw)
    : app_state_(app_state), db_controller_(db_controller), trigger_redraw_(trigger_redraw), current_search_id_(0) {}

void LocationController::Search(const std::string& query) {
    {
        std::lock_guard<std::mutex> lock(search_state_.mutex);
        search_state_.search_query = query;
        if (query.empty()) {
            search_state_.search_suggestions.clear();
            search_state_.selected_suggestion_index = 0;
            search_state_.is_loading = false;
            search_state_.has_error = false;
            search_state_.error_message = "";
            if (trigger_redraw_) {
                trigger_redraw_();
            }
            return;
        }

        search_state_.is_loading = true;
        search_state_.has_error = false;
        search_state_.error_message = "";
    }

    uint64_t search_id = ++current_search_id_;

    // Dispatch geocoding query onto background worker thread
    std::thread([this, query, search_id]() {
        try {
            auto matches = GeocodingService::Search(query);

            if (search_id == current_search_id_) {
                std::lock_guard<std::mutex> lock(search_state_.mutex);
                search_state_.search_suggestions = matches;
                search_state_.selected_suggestion_index = 0;
                search_state_.is_loading = false;

                if (trigger_redraw_) {
                    trigger_redraw_();
                }
            }
        } catch (const std::exception& e) {
            if (search_id == current_search_id_) {
                std::lock_guard<std::mutex> lock(search_state_.mutex);
                search_state_.search_suggestions.clear();
                search_state_.selected_suggestion_index = 0;
                search_state_.is_loading = false;
                search_state_.has_error = true;
                search_state_.error_message = e.what();

                if (trigger_redraw_) {
                    trigger_redraw_();
                }
            }
        }
    }).detach();
}

void LocationController::SelectSuggestion(int index) {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    if (index >= 0 && index < static_cast<int>(search_state_.search_suggestions.size())) {
        const auto& match = search_state_.search_suggestions[index];
        
        // Write results to main app state
        app_state_.city_name = match.name;
        app_state_.latitude = match.latitude;
        app_state_.longitude = match.longitude;

        if (search_state_.save_to_db) {
            db_controller_.SaveLocation(match);
        }

        // Reset query modal parameters
        search_state_.show_search_modal = false;
        search_state_.save_to_db = false;
        search_state_.search_query = "";
        search_state_.cursor_position = 0;
        search_state_.search_suggestions.clear();
        search_state_.selected_suggestion_index = 0;
        search_state_.has_error = false;
        search_state_.error_message = "";

        if (trigger_redraw_) {
            trigger_redraw_();
        }
    }
}

void LocationController::CancelSearch() {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    search_state_.show_search_modal = false;
    search_state_.save_to_db = false;
    search_state_.search_query = "";
    search_state_.cursor_position = 0;
    search_state_.search_suggestions.clear();
    search_state_.selected_suggestion_index = 0;
    search_state_.has_error = false;
    search_state_.error_message = "";

    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

void LocationController::OpenSearch() {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    search_state_.show_search_modal = true;
    search_state_.save_to_db = false;
    search_state_.search_query = "";
    search_state_.cursor_position = 0;
    search_state_.search_suggestions.clear();
    search_state_.selected_suggestion_index = 0;
    search_state_.has_error = false;
    search_state_.error_message = "";

    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

}  // namespace weather_cli
