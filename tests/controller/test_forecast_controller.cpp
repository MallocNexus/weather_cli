#include <catch2/catch_test_macros.hpp>
#include "controller/forecast_controller.hpp"
#include "model/app_state.hpp"
#include <chrono>
#include <thread>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Spin-waits up to max_ms milliseconds until pred() returns true.
// Returns true if pred was satisfied before the timeout, false otherwise.
template <typename Pred>
static bool WaitFor(Pred pred, int max_ms = 3000) {
    const auto deadline = std::chrono::steady_clock::now() +
                          std::chrono::milliseconds(max_ms);
    while (!pred()) {
        if (std::chrono::steady_clock::now() >= deadline) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return true;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST_CASE("ForecastController::Refresh", "[controller][forecast]") {
    using namespace weather_cli;

    AppState state;
    int redraw_count = 0;
    ForecastController controller(state, [&redraw_count]() { ++redraw_count; });

    SECTION("Refresh sets is_loading = true synchronously") {
        REQUIRE_FALSE(state.is_loading);
        controller.Refresh();
        // is_loading is set before the thread is dispatched
        REQUIRE(state.is_loading);
    }

    SECTION("Refresh clears is_loading after completion (success or failure)") {
        controller.Refresh();
        // Wait up to 3 s for the background fetch to finish.
        // In CI / offline environments the fetch fails gracefully and
        // ForecastController still clears is_loading on error.
        bool finished = WaitFor([&state]() { return !state.is_loading; });
        REQUIRE(finished);
    }

    SECTION("Refresh triggers the redraw callback after completion") {
        controller.Refresh();
        bool got_redraw = WaitFor([&redraw_count]() { return redraw_count > 0; });
        REQUIRE(got_redraw);
    }

    SECTION("Refresh updates has_error or current_conditions (not both)") {
        controller.Refresh();
        WaitFor([&state]() { return !state.is_loading; });

        // Exactly one of the two outcomes: success OR error — never both.
        const bool success = state.current_conditions.has_value();
        const bool error   = state.has_error;
        REQUIRE((success || error));         // one must be set
        REQUIRE_FALSE((success && error));   // but not both
    }

    SECTION("AppState defaults: current_conditions starts as nullopt") {
        // Verify the starting condition before any Refresh call.
        AppState fresh_state;
        REQUIRE_FALSE(fresh_state.current_conditions.has_value());
        REQUIRE_FALSE(fresh_state.is_loading);
    }
}
