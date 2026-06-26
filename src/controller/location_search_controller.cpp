#include "controller/location_search_controller.hpp"
#include "service/geocoding_service.hpp"
#include <thread>
#include <mutex>

namespace weather_cli {

LocationSearchController::LocationSearchController(LocationController& location_controller, std::function<void()> trigger_redraw)
    : location_controller_(location_controller), trigger_redraw_(trigger_redraw), current_search_id_(0) {}

void LocationSearchController::Search(const std::string& query) {
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
        search_state_.search_suggestions.clear();
        search_state_.selected_suggestion_index = 0;
        search_state_.has_error = false;
        search_state_.error_message = "";
    }

    if (trigger_redraw_) {
        trigger_redraw_();
    }

    uint64_t search_id = ++current_search_id_;

    const std::string country = search_state_.country_filter;

    // Dispatch geocoding query onto background worker thread
    std::thread([this, query, country, search_id]() {
        try {
            auto matches = GeocodingService::Search(query, country);

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

void LocationSearchController::SelectSuggestion(int index) {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    if (index >= 0 && index < static_cast<int>(search_state_.search_suggestions.size())) {
        const auto& match = search_state_.search_suggestions[index];
        
        // Delegate coordinates setting and db saving to LocationController
        location_controller_.SelectLocation(match, search_state_.save_to_db);

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

void LocationSearchController::CancelSearch() {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    search_state_.show_search_modal = false;
    search_state_.save_to_db = false;
    search_state_.search_query = "";
    search_state_.cursor_position = 0;
    search_state_.search_suggestions.clear();
    search_state_.selected_suggestion_index = 0;
    search_state_.has_error = false;
    search_state_.error_message = "";
    search_state_.country_filter_index = 0;
    search_state_.country_filter = std::string(kCountryList[0].code);

    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

void LocationSearchController::OpenSearch() {
    std::lock_guard<std::mutex> lock(search_state_.mutex);
    search_state_.show_search_modal = true;
    search_state_.save_to_db = false;
    search_state_.search_query = "";
    search_state_.cursor_position = 0;
    search_state_.search_suggestions.clear();
    search_state_.selected_suggestion_index = 0;
    search_state_.has_error = false;
    search_state_.error_message = "";
    search_state_.country_filter_index = 0;
    search_state_.country_filter = std::string(kCountryList[0].code);

    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

void LocationSearchController::SetCountryFilter(int index) {
    if (index < 0 || index >= static_cast<int>(kCountryList.size())) {
        return;
    }

    std::string active_query;
    {
        std::lock_guard<std::mutex> lock(search_state_.mutex);
        search_state_.country_filter_index = index;
        search_state_.country_filter = std::string(kCountryList[index].code);
        active_query = search_state_.search_query;
    }

    if (!active_query.empty()) {
        Search(active_query);
    }
}

void LocationSearchController::TriggerSearch() {
    std::string query;
    {
        std::lock_guard<std::mutex> lock(search_state_.mutex);
        query = search_state_.search_query;
    }
    Search(query);
}

}  // namespace weather_cli
