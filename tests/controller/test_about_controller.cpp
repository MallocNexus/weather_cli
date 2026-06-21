#include <catch2/catch_test_macros.hpp>
#include "controller/about_controller.hpp"

TEST_CASE("AboutController Actions", "[controller][about]") {
    using namespace weather_cli;

    AboutState state;
    bool redraw_triggered = false;
    auto trigger_redraw = [&]() { redraw_triggered = true; };

    AboutController controller(state, trigger_redraw);
    auto& about_state = controller.GetAboutState();

    SECTION("Initial state is clean") {
        REQUIRE_FALSE(about_state.show_about_modal);
        REQUIRE_FALSE(redraw_triggered);
    }

    SECTION("OpenAbout activates modal and triggers redraw") {
        controller.OpenAbout();
        REQUIRE(about_state.show_about_modal);
        REQUIRE(redraw_triggered);
    }

    SECTION("CloseAbout deactivates modal and triggers redraw") {
        // Preset state
        about_state.show_about_modal = true;
        redraw_triggered = false;

        controller.CloseAbout();
        REQUIRE_FALSE(about_state.show_about_modal);
        REQUIRE(redraw_triggered);
    }
}
