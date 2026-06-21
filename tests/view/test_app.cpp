#include <catch2/catch_test_macros.hpp>
#include "view/app.hpp"
#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "controller/about_controller.hpp"
#include "model/app_state.hpp"
#include "model/about_state.hpp"

TEST_CASE("App View Layout Component Verification", "[view][app]") {
    using namespace weather_cli;

    AppState state;
    LocationController loc_controller(state, []() {});
    AboutState about_state;
    AboutController about_controller(about_state, []() {});
    AppController controller(state, loc_controller, about_controller, []() {});

    SECTION("App view constructor and component retrieval succeed") {
        App app(state, controller);
        auto comp = app.GetComponent();
        REQUIRE(comp != nullptr);
    }
}
