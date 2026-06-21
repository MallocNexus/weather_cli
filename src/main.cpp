#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "model/app_state.hpp"
#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "view/app.hpp"

int main(int argc, char* argv[]) {
    using namespace weather_cli;

    std::string area_code = "";
    std::string country = "";

    // Parse CLI parameters
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--stdin-file" && i + 1 < argc) {
            std::freopen(argv[i + 1], "r", stdin);
            i++;
        } else if ((arg == "--area-code" || arg == "-a") && i + 1 < argc) {
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

    // Scenario 3: Real Interactive TUI Setup
    try {
        auto screen = ftxui::ScreenInteractive::Fullscreen();

        AppState state;
        
        LocationController loc_controller(state, [&screen] {
            screen.PostEvent(ftxui::Event::Custom);
        });

        AppController controller(state, loc_controller, screen.ExitLoopClosure());

        App app(state, controller);

        screen.Loop(app.GetComponent());
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Fatal TUI Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}

