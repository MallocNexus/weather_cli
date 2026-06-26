#include <catch2/catch_test_macros.hpp>
#include "controller/location_controller.hpp"
#include "controller/db_controller.hpp"
#include "model/location_repository.hpp"
#include <chrono>
#include <thread>

TEST_CASE("LocationController Actions", "[controller][location]") {
    using namespace weather_cli;

    AppState app_state;
    bool redraw_triggered = false;
    auto trigger_redraw = [&]() { redraw_triggered = true; };

    LocationRepository repo(":memory:");
    repo.Initialize();
    DatabaseController db_controller(repo, []() {});
    LocationController controller(app_state, db_controller, trigger_redraw);
    auto& search_state = controller.GetSearchState();

    SECTION("Initial state is clean") {
        REQUIRE_FALSE(search_state.show_search_modal);
        REQUIRE(search_state.search_query.empty());
        REQUIRE(search_state.search_suggestions.empty());
    }

    SECTION("OpenSearch configures modal visibility and resets query") {
        search_state.show_search_modal = false;
        search_state.search_query = "Berlin";
        search_state.search_suggestions.push_back(LocationMatch{"Berlin", "Germany", "", 52.52, 13.40});
        search_state.selected_suggestion_index = 2;
        redraw_triggered = false;

        controller.OpenSearch();

        REQUIRE(search_state.show_search_modal);
        REQUIRE(search_state.search_query.empty());
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE(search_state.selected_suggestion_index == 0);
        REQUIRE(redraw_triggered);
    }

    SECTION("Empty search query clears state synchronously") {
        search_state.search_query = "Berlin";
        search_state.search_suggestions.push_back(LocationMatch{"Berlin", "Germany", "", 52.52, 13.40});
        search_state.is_loading = true;
        redraw_triggered = false;

        controller.Search("");

        REQUIRE(search_state.search_query.empty());
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE_FALSE(search_state.is_loading);
        REQUIRE(redraw_triggered);
    }

    SECTION("SelectSuggestion updates active coordinates and resets modal state") {
        search_state.show_search_modal = true;
        search_state.search_query = "Ber";
        search_state.search_suggestions.push_back(LocationMatch{"Berlin", "Germany", "Berlin State", 52.52, 13.40});
        search_state.selected_suggestion_index = 0;
        redraw_triggered = false;

        controller.SelectSuggestion(0);

        REQUIRE(app_state.city_name == "Berlin");
        REQUIRE(app_state.latitude == 52.52);
        REQUIRE(app_state.longitude == 13.40);

        // Reset verification
        REQUIRE_FALSE(search_state.show_search_modal);
        REQUIRE(search_state.search_query.empty());
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE(search_state.selected_suggestion_index == 0);
        REQUIRE(redraw_triggered);
    }

    SECTION("CancelSearch resets modal state") {
        search_state.show_search_modal = true;
        search_state.search_query = "Tokyo";
        search_state.search_suggestions.push_back(LocationMatch{"Tokyo", "Japan", "", 35.6762, 139.6503});
        search_state.selected_suggestion_index = 2;
        search_state.has_error = true;
        search_state.error_message = "Some error";
        redraw_triggered = false;

        controller.CancelSearch();

        REQUIRE_FALSE(search_state.show_search_modal);
        REQUIRE(search_state.search_query.empty());
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE(search_state.selected_suggestion_index == 0);
        REQUIRE_FALSE(search_state.has_error);
        REQUIRE(search_state.error_message.empty());
        REQUIRE(redraw_triggered);
    }

    // -----------------------------------------------------------------------
    // SetCountryFilter
    // -----------------------------------------------------------------------

    SECTION("SetCountryFilter(0) sets country_filter to AU and index to 0") {
        // Dirty the state first so we know the write actually happened.
        search_state.country_filter       = "XX";
        search_state.country_filter_index = 99;

        controller.SetCountryFilter(0);

        REQUIRE(search_state.country_filter       == "AU");
        REQUIRE(search_state.country_filter_index == 0);
    }

    SECTION("SetCountryFilter to US index sets country_filter to US") {
        // Find the US index in kCountryList so the test isn't brittle to list reordering.
        int us_index = -1;
        for (int i = 0; i < static_cast<int>(kCountryList.size()); ++i) {
            if (kCountryList[i].code == "US") { us_index = i; break; }
        }
        REQUIRE(us_index >= 0);  // sanity: US must be in the list

        controller.SetCountryFilter(us_index);

        REQUIRE(search_state.country_filter       == "US");
        REQUIRE(search_state.country_filter_index == us_index);
    }

    SECTION("SetCountryFilter to Any Country index sets country_filter to empty string") {
        // Find the "Any Country" entry (empty code).
        int any_index = -1;
        for (int i = 0; i < static_cast<int>(kCountryList.size()); ++i) {
            if (kCountryList[i].code.empty()) { any_index = i; break; }
        }
        REQUIRE(any_index >= 0);

        controller.SetCountryFilter(any_index);

        REQUIRE(search_state.country_filter.empty());
        REQUIRE(search_state.country_filter_index == any_index);
    }

    SECTION("SetCountryFilter with negative index is a no-op") {
        search_state.country_filter       = "AU";
        search_state.country_filter_index = 0;

        controller.SetCountryFilter(-1);

        REQUIRE(search_state.country_filter       == "AU");
        REQUIRE(search_state.country_filter_index == 0);
    }

    SECTION("SetCountryFilter with index >= list size is a no-op") {
        search_state.country_filter       = "AU";
        search_state.country_filter_index = 0;

        controller.SetCountryFilter(static_cast<int>(kCountryList.size()));

        REQUIRE(search_state.country_filter       == "AU");
        REQUIRE(search_state.country_filter_index == 0);
    }

    SECTION("OpenSearch resets country filter to AU / index 0") {
        // Simulate a previous session where the user picked Japan.
        int jp_index = -1;
        for (int i = 0; i < static_cast<int>(kCountryList.size()); ++i) {
            if (kCountryList[i].code == "JP") { jp_index = i; break; }
        }
        REQUIRE(jp_index >= 0);
        controller.SetCountryFilter(jp_index);
        REQUIRE(search_state.country_filter == "JP");

        controller.OpenSearch();

        REQUIRE(search_state.country_filter       == "AU");
        REQUIRE(search_state.country_filter_index == 0);
    }

    SECTION("CancelSearch resets country filter to AU / index 0") {
        int jp_index = -1;
        for (int i = 0; i < static_cast<int>(kCountryList.size()); ++i) {
            if (kCountryList[i].code == "JP") { jp_index = i; break; }
        }
        REQUIRE(jp_index >= 0);
        controller.SetCountryFilter(jp_index);
        REQUIRE(search_state.country_filter == "JP");

        controller.CancelSearch();

        REQUIRE(search_state.country_filter       == "AU");
        REQUIRE(search_state.country_filter_index == 0);
    }

    SECTION("TriggerSearch with non-empty query sets is_loading = true and clears suggestions") {
        search_state.search_query = "Sydney";
        search_state.search_suggestions.push_back(LocationMatch{"Berlin", "Germany", "", 52.52, 13.40});
        search_state.is_loading   = false;
        redraw_triggered          = false;

        controller.TriggerSearch();

        REQUIRE(search_state.is_loading == true);
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE(search_state.search_query == "Sydney");
        REQUIRE(redraw_triggered);

        // Clean up any async tasks/background thread
        controller.CancelSearch();
    }

    SECTION("TriggerSearch with empty query is a no-op / clears state") {
        search_state.search_query = "";
        search_state.search_suggestions.push_back(LocationMatch{"Berlin", "Germany", "", 52.52, 13.40});
        search_state.is_loading   = true;
        redraw_triggered          = false;

        controller.TriggerSearch();

        REQUIRE(search_state.is_loading == false);
        REQUIRE(search_state.search_suggestions.empty());
        REQUIRE(search_state.search_query.empty());
        REQUIRE(redraw_triggered);
    }
}
