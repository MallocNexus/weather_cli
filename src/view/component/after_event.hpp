#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <functional>

namespace weather_cli {

class AfterEvent : public ftxui::ComponentBase {
public:
    AfterEvent(ftxui::Component child, std::function<void(ftxui::Event)> after_event);
    bool OnEvent(ftxui::Event event) override;

private:
    std::function<void(ftxui::Event)> after_event_;
};

ftxui::Component OnAfterEvent(ftxui::Component child, std::function<void(ftxui::Event)> after_event);

} // namespace weather_cli
