#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main(int argc, char* argv[]) {
    std::string area_code = "";
    std::string country = "";

    // Parse CLI parameters
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--area-code" || arg == "-a") && i + 1 < argc) {
            area_code = argv[i + 1];
            i++;
        } else if ((arg == "--country" || arg == "-c") && i + 1 < argc) {
            country = argv[i + 1];
            i++;
        }
    }

    // Scenario 1: Headless CLI Parameters (e.g. --area-code 2155 --country AUS)
    if (!area_code.empty() || !country.empty()) {
        std::cout << "Area Code: " << area_code << ", Country: " << country << "\n";
        return EXIT_SUCCESS;
    }

    // Scenario 2: Headless Stdin Pipe (e.g. echo 2155 AUS | weather-cli)
    if (!isatty(STDIN_FILENO)) {
        std::string input;
        if (std::getline(std::cin, input) && !input.empty()) {
            std::stringstream ss(input);
            ss >> area_code >> country;
            std::cout << "Area Code: " << area_code << ", Country: " << country << "\n";
            return EXIT_SUCCESS;
        }
    }

    // Scenario 3: Interactive TUI Stub (launches a menu to just quit)
    using namespace ftxui;
    auto screen = ScreenInteractive::FitComponent();

    int selected = 0;
    std::vector<std::string> menu_entries = {
        "Quit"
    };

    auto menu = Menu(&menu_entries, &selected);
    auto component = Renderer(menu, [&] {
        return vbox({
            text("Quick Weather CLI App (Stub)") | bold | center,
            separator(),
            text("Active Location: None (TUI Stub Mode)"),
            separator(),
            menu->Render() | border,
        });
    });

    auto closure = screen.ExitLoopClosure();
    auto component_with_quit = CatchEvent(component, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape || (selected == 0 && event == Event::Return)) {
            closure();
            return true;
        }
        return false;
    });

    screen.Loop(component_with_quit);
    return EXIT_SUCCESS;
}
