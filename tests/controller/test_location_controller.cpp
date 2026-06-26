#include <catch2/catch_test_macros.hpp>
#include "controller/location_controller.hpp"
#include "controller/db_controller.hpp"
#include "model/location_repository.hpp"

TEST_CASE("LocationController Actions", "[controller][location]") {
    using namespace weather_cli;

    SECTION("SelectLocation updates active coordinates and delegates DB save if requested") {
        AppState app_state;
        bool redraw_triggered = false;
        auto trigger_redraw = [&]() { redraw_triggered = true; };

        LocationRepository repo(":memory:");
        repo.Initialize();
        DatabaseController db_controller(repo, []() {});
        LocationController controller(app_state, db_controller, trigger_redraw);

        LocationMatch match{"Berlin", "Germany", "Berlin State", 52.52, 13.40};
        controller.SelectLocation(match, true);

        REQUIRE(app_state.city_name == "Berlin");
        REQUIRE(app_state.latitude == 52.52);
        REQUIRE(app_state.longitude == 13.40);
        REQUIRE(redraw_triggered);

        // Verify it was saved to DB repository
        const auto& saved = repo.GetSavedLocations();
        REQUIRE(saved.size() == 1);
        REQUIRE(saved.front().name == "Berlin");
    }

    SECTION("SelectLocation does not save to DB if save_to_db is false") {
        AppState app_state;
        bool redraw_triggered = false;
        auto trigger_redraw = [&]() { redraw_triggered = true; };

        LocationRepository repo(":memory:");
        repo.Initialize();
        DatabaseController db_controller(repo, []() {});
        LocationController controller(app_state, db_controller, trigger_redraw);

        LocationMatch match{"Tokyo", "Japan", "", 35.67, 139.65};
        controller.SelectLocation(match, false);

        REQUIRE(app_state.city_name == "Tokyo");
        REQUIRE(app_state.latitude == 35.67);
        REQUIRE(app_state.longitude == 139.65);
        REQUIRE(redraw_triggered);

        // Verify it was NOT saved to DB repository
        const auto& saved = repo.GetSavedLocations();
        REQUIRE(saved.empty());
    }
}
