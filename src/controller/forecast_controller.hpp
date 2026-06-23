#pragma once

#include "model/app_state.hpp"
#include <atomic>
#include <functional>

namespace weather_cli {

// ForecastController owns the lifecycle of background weather fetches.
// It follows the same async + atomic-sequence-counter pattern as
// LocationController to safely discard stale responses when a new Refresh()
// is triggered before an in-flight request completes.
class ForecastController {
public:
    // trigger_redraw is called on the TUI thread after each successful or
    // failed fetch so FTXUI repaints with the new state.
    ForecastController(AppState& state, std::function<void()> trigger_redraw);

    // Triggers a background fetch of current conditions for the coordinates
    // currently stored in state_.latitude / state_.longitude.
    // Safe to call from the TUI thread — all blocking I/O runs off-thread.
    void Refresh();

private:
    AppState& state_;
    std::function<void()> trigger_redraw_;

    // Monotonically increasing counter. Incremented on every Refresh() call.
    // Background threads capture their issued ID and discard results when
    // current_fetch_id_ has advanced past them (i.e. a newer call won).
    std::atomic<uint64_t> current_fetch_id_{0};
};

}  // namespace weather_cli
