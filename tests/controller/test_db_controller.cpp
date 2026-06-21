#include <catch2/catch_test_macros.hpp>
#include "controller/db_controller.hpp"
#include "model/location_repository.hpp"
#include "util/constants.hpp"
#include <sqlite3.h>

TEST_CASE("DatabaseController Actions", "[controller][database]") {
    using namespace weather_cli;

    LocationRepository repo(":memory:");
    bool redraw_triggered = false;
    auto trigger_redraw = [&]() { redraw_triggered = true; };

    DatabaseController controller(repo, trigger_redraw);

    // Setup: Initialize DB
    REQUIRE(controller.InitializeDatabase());

    SECTION("Initial loaded repository is empty") {
        redraw_triggered = false;
        REQUIRE(controller.LoadSavedLocations());
        REQUIRE(repo.GetSavedLocations().empty());
        REQUIRE(redraw_triggered);
    }

    SECTION("SaveLocation adds a location and reloads state") {
        LocationMatch loc1{"Berlin", "Germany", "Berlin State", 52.52, 13.40};
        redraw_triggered = false;

        REQUIRE(controller.SaveLocation(loc1));
        const auto& saved = repo.GetSavedLocations();
        REQUIRE(saved.size() == 1);
        REQUIRE(saved[0].name == "Berlin");
        REQUIRE(saved[0].country == "Germany");
        REQUIRE(saved[0].latitude == 52.52);
        REQUIRE(redraw_triggered);
    }

    SECTION("Duplicate SaveLocation is ignored") {
        LocationMatch loc1{"Berlin", "Germany", "Berlin State", 52.52, 13.40};
        REQUIRE(controller.SaveLocation(loc1));
        REQUIRE(repo.GetSavedLocations().size() == 1);

        redraw_triggered = false;
        REQUIRE(controller.SaveLocation(loc1)); // Attempt duplicate save
        REQUIRE(repo.GetSavedLocations().size() == 1); // Size should still be 1
    }
}
