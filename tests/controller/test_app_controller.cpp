#include <catch2/catch_test_macros.hpp>
#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "controller/about_controller.hpp"
#include "model/app_state.hpp"
#include "model/about_state.hpp"

TEST_CASE("AppController Actions", "[controller][app]") {
    using namespace weather_cli;

    AppState state;
    bool quit_called = false;
    auto on_quit = [&]() { quit_called = true; };

    LocationController location_controller(state, []() {});
    AboutState about_state;
    AboutController about_controller(about_state, []() {});
    AppController controller(state, location_controller, about_controller, on_quit);

    SECTION("Initial AppState configuration matches defaults") {
        REQUIRE(state.is_celsius);
        REQUIRE(state.active_tab == ViewTab::kHourlyGraph);
        REQUIRE(state.selected_hour_index == 9);
        REQUIRE(state.city_name == "Sydney");
        REQUIRE_FALSE(quit_called);
    }

    SECTION("ToggleUnits flips Celsius/Fahrenheit setting") {
        controller.ToggleUnits();
        REQUIRE_FALSE(state.is_celsius);

        controller.ToggleUnits();
        REQUIRE(state.is_celsius);
    }

    SECTION("SetActiveTab changes active tab") {
        controller.SetActiveTab(ViewTab::kRainGraph);
        REQUIRE(state.active_tab == ViewTab::kRainGraph);

        controller.SetActiveTab(ViewTab::kHourlyGraph);
        REQUIRE(state.active_tab == ViewTab::kHourlyGraph);
    }

    SECTION("UpdateSliderIndex updates selected hour index") {
        controller.UpdateSliderIndex(15);
        REQUIRE(state.selected_hour_index == 15);
    }

    SECTION("RefreshForecast runs (temporary stub test)") {
        controller.RefreshForecast();
        REQUIRE_FALSE(state.is_loading);
    }

    SECTION("SearchCity updates city name in state") {
        controller.SearchCity("Berlin");
        REQUIRE(state.city_name == "Berlin");
    }

    SECTION("Quit invokes the registered exit loop closure callback") {
        controller.Quit();
        REQUIRE(quit_called);
    }

    SECTION("OpenSearch delegates modal state update to LocationController") {
        auto& search_state = location_controller.GetSearchState();
        REQUIRE_FALSE(search_state.show_search_modal);

        controller.OpenSearch();
        REQUIRE(search_state.show_search_modal);
    }

    SECTION("IsSearchModalOpen returns thread-safe state from LocationController") {
        auto& search_state = location_controller.GetSearchState();

        {
            std::lock_guard<std::mutex> lock(search_state.mutex);
            search_state.show_search_modal = true;
        }
        REQUIRE(controller.IsSearchModalOpen());

        {
            std::lock_guard<std::mutex> lock(search_state.mutex);
            search_state.show_search_modal = false;
        }
        REQUIRE_FALSE(controller.IsSearchModalOpen());
    }

    SECTION("SelectPresetLocation updates state parameters") {
        // Preset 1: Sydney
        controller.SelectPresetLocation(1);
        REQUIRE(state.city_name == "Sydney");
        REQUIRE(state.latitude == -33.8688);
        REQUIRE(state.longitude == 151.2093);

        // Preset 2: New York
        controller.SelectPresetLocation(2);
        REQUIRE(state.city_name == "New York");
        REQUIRE(state.latitude == 40.7128);
        REQUIRE(state.longitude == -74.0060);
    }

    SECTION("GetLocationController returns internal controller reference") {
        REQUIRE(&controller.GetLocationController() == &location_controller);

        const auto& const_controller = controller;
        REQUIRE(&const_controller.GetLocationController() == &location_controller);
    }

    SECTION("OpenAbout delegates modal state update to AboutController") {
        auto& current_about_state = about_controller.GetAboutState();
        REQUIRE_FALSE(current_about_state.show_about_modal);

        controller.OpenAbout();
        REQUIRE(current_about_state.show_about_modal);
    }

    SECTION("IsAboutModalOpen returns state from AboutController") {
        auto& current_about_state = about_controller.GetAboutState();
        current_about_state.show_about_modal = true;
        REQUIRE(controller.IsAboutModalOpen());

        current_about_state.show_about_modal = false;
        REQUIRE_FALSE(controller.IsAboutModalOpen());
    }

    SECTION("GetAboutController returns internal controller reference") {
        REQUIRE(&controller.GetAboutController() == &about_controller);

        const auto& const_controller = controller;
        REQUIRE(&const_controller.GetAboutController() == &about_controller);
    }
}
