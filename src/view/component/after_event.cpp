#include "view/component/after_event.hpp"

namespace weather_cli {

AfterEvent::AfterEvent(ftxui::Component child, std::function<void(ftxui::Event)> after_event)
    : after_event_(std::move(after_event)) {
    Add(std::move(child));
}

bool AfterEvent::OnEvent(ftxui::Event event) {
    bool handled = ComponentBase::OnEvent(event);
    if (handled) {
        after_event_(event);
    }
    return handled;
}

ftxui::Component OnAfterEvent(ftxui::Component child, std::function<void(ftxui::Event)> after_event) {
    return ftxui::Make<AfterEvent>(std::move(child), std::move(after_event));
}

} // namespace weather_cli
