#include <catch2/catch_test_macros.hpp>
#include "controller/location_controller.hpp"
#include <chrono>
#include <thread>

TEST_CASE("LocationController Actions", "[controller][location]") {
    using namespace weather_cli;

    AppState app_state;
    bool redraw_triggered = false;
    auto trigger_redraw = [&]() { redraw_triggered = true; };

    LocationController controller(app_state, trigger_redraw);
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
}
