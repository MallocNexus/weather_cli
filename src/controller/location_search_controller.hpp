#pragma once

#include "controller/location_controller.hpp"
#include "model/location_search_state.hpp"
#include "util/constants.hpp"
#include <functional>
#include <string>
#include <atomic>

namespace weather_cli {

class LocationSearchController {
public:
    LocationSearchController(LocationController& location_controller, std::function<void()> trigger_redraw);

    // Invokes background thread GeocodingService calls
    void Search(const std::string& query);

    // Selects suggestion and updates coordinates state
    void SelectSuggestion(int index);

    // Cancels/resets current search state and closes modal
    void CancelSearch();

    // Opens the search modal
    void OpenSearch();

    // Updates the country filter used by subsequent Search() calls.
    void SetCountryFilter(int index);

    // Triggers a search based on the current query in the search state.
    void TriggerSearch();

    // Accessor for the search state
    LocationSearchState& GetSearchState() { return search_state_; }
    const LocationSearchState& GetSearchState() const { return search_state_; }

private:
    LocationController& location_controller_;
    std::function<void()> trigger_redraw_;
    LocationSearchState search_state_;
    std::atomic<uint64_t> current_search_id_{0};
};

}  // namespace weather_cli
