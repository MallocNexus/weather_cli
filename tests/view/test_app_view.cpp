#include <catch2/catch_test_macros.hpp>
#include "view/app_view.hpp"
#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "controller/location_search_controller.hpp"
#include "controller/about_controller.hpp"
#include "controller/db_controller.hpp"
#include "controller/forecast_controller.hpp"
#include "model/app_state.hpp"
#include "model/about_state.hpp"
#include "model/location_repository.hpp"

TEST_CASE("AppView Layout Component Verification", "[view][app]") {
    using namespace weather_cli;

    AppState state;
    LocationRepository repo(":memory:");
    repo.Initialize();
    DatabaseController db_controller(repo, []() {});
    LocationController loc_controller(state, db_controller, []() {});
    LocationSearchController loc_search_controller(loc_controller, []() {});
    AboutState about_state;
    AboutController about_controller(about_state, []() {});
    ForecastController forecast_controller(state, []() {});
    AppController controller(state, loc_controller, loc_search_controller, about_controller, db_controller, forecast_controller, []() {});

    SECTION("AppView constructor and component retrieval succeed") {
        AppView app(state, controller);
        auto comp = app.GetComponent();
        REQUIRE(comp != nullptr);
    }
}
