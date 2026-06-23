#include "controller/forecast_controller.hpp"
#include "service/weather_service.hpp"
#include <thread>

namespace weather_cli {

ForecastController::ForecastController(AppState& state,
                                       std::function<void()> trigger_redraw)
    : state_(state), trigger_redraw_(trigger_redraw), current_fetch_id_(0) {}

void ForecastController::Refresh() {
    state_.is_loading = true;
    state_.has_error  = false;
    state_.error_message = "";

    // Snapshot the coordinates at dispatch time so the lambda captures a
    // consistent lat/lon even if the user navigates to a different location
    // before the request completes.
    const double lat = state_.latitude;
    const double lon = state_.longitude;

    // Increment the sequence counter and capture this fetch's ID.
    const uint64_t fetch_id = ++current_fetch_id_;

    std::thread([this, lat, lon, fetch_id]() {
        auto result = WeatherService::FetchCurrentConditions(lat, lon);

        // Discard the response if a newer Refresh() call has already been
        // issued — the UI will be updated by the newer fetch instead.
        if (fetch_id != current_fetch_id_) {
            return;
        }

        if (result.has_value()) {
            state_.current_conditions = result;
            state_.is_loading = false;
        } else {
            state_.current_conditions = std::nullopt;
            state_.is_loading = false;
            state_.has_error = true;
            state_.error_message = "Failed to fetch weather data.";
        }

        if (trigger_redraw_) {
            trigger_redraw_();
        }
    }).detach();
}

}  // namespace weather_cli
