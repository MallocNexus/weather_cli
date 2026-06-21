#include "controller/about_controller.hpp"

namespace weather_cli {

AboutController::AboutController(AboutState& state, std::function<void()> trigger_redraw)
    : state_(state), trigger_redraw_(trigger_redraw) {}

void AboutController::OpenAbout() {
    state_.show_about_modal = true;
    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

void AboutController::CloseAbout() {
    state_.show_about_modal = false;
    if (trigger_redraw_) {
        trigger_redraw_();
    }
}

}  // namespace weather_cli
