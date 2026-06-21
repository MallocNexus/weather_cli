#include "view/about_view.hpp"
#include "util/constants.hpp"
#include <ftxui/component/event.hpp>

using namespace ftxui;

namespace weather_cli {

AboutView::AboutView(AboutController& controller)
    : controller_(controller) {

    // Clickable close button
    close_button_ = Button("Close", [this] {
        controller_.CloseAbout();
    });

    // Intercept Escape key to dismiss the modal
    auto event_dispatcher = CatchEvent(close_button_, [this](Event event) {
        if (controller_.GetAboutState().show_about_modal) {
            if (event == Event::Escape) {
                controller_.CloseAbout();
                return true;
            }
        }
        return false;
    });

    // Render layout
    view_component_ = Renderer(event_dispatcher, [this] {
        auto modal_element = vbox({
            text(kAppName) | bold | center | color(Color::Green),
            text("Version: " + kAppVersion) | center | dim,
            separator(),
            text("A responsive terminal-based weather client") | center,
            text("built with C++ and FTXUI.") | center,
            separator(),
            close_button_->Render() | center | size(WIDTH, EQUAL, 12),
        }) | size(WIDTH, EQUAL, 50) | border | color(Color::Green) | clear_under | center;

        return modal_element;
    });
}

Component AboutView::GetComponent() {
    return view_component_;
}

Element AboutView::Render(Element base_document) {
    if (!controller_.GetAboutState().show_about_modal) {
        return base_document;
    }

    return dbox({
        base_document,
        view_component_->Render()
    });
}

}  // namespace weather_cli
