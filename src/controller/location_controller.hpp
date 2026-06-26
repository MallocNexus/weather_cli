#pragma once

#include "model/app_state.hpp"
#include "model/location_search_state.hpp"
#include "util/constants.hpp"
#include <functional>
#include <string>
#include <atomic>

namespace weather_cli {

class DatabaseController;

class LocationController {
public:
    LocationController(AppState& app_state, DatabaseController& db_controller, std::function<void()> trigger_redraw);

    // Invokes background thread GeocodingService calls
    void Search(const std::string& query);

    // Selects suggestion and updates coordinates state
    void SelectSuggestion(int index);

    // Cancels/resets current search state and closes modal
    void CancelSearch();

    // Opens the search modal
    void OpenSearch();

    // Updates the country filter used by subsequent Search() calls.
    // index must be a valid index into kCountryList — out-of-bounds is a no-op.
    // If a non-empty query is already in the search box the results are
    // immediately re-fetched with the new country constraint.
    void SetCountryFilter(int index);

    // Triggers a search based on the current query in the search state.
    void TriggerSearch();

    // Accessor for the search state
    LocationSearchState& GetSearchState() { return search_state_; }
    const LocationSearchState& GetSearchState() const { return search_state_; }

private:
    AppState& app_state_;
    DatabaseController& db_controller_;
    std::function<void()> trigger_redraw_;
    LocationSearchState search_state_;
    std::atomic<uint64_t> current_search_id_{0};
};

}  // namespace weather_cli
