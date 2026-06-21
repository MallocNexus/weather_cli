#pragma once

#include "model/about_state.hpp"
#include <functional>

namespace weather_cli {

class AboutController {
public:
    AboutController(AboutState& state, std::function<void()> trigger_redraw);

    void OpenAbout();
    void CloseAbout();

    AboutState& GetAboutState() { return state_; }
    const AboutState& GetAboutState() const { return state_; }

private:
    AboutState& state_;
    std::function<void()> trigger_redraw_;
};

}  // namespace weather_cli
