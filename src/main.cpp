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
#include "model/about_state.hpp"
#include "model/location_repository.hpp"
#include "controller/app_controller.hpp"
#include "controller/location_controller.hpp"
#include "controller/about_controller.hpp"
#include "controller/db_controller.hpp"
#include "view/app.hpp"
#include "util/constants.hpp"

int main(int argc, char* argv[]) {
    using namespace weather_cli;

    std::string area_code = "";
    std::string country = "";

    // Scenario 1 & 2: Headless arguments or Pipe/Stdin input handling
    if (argc >= 2 || !isatty(STDIN_FILENO)) {
        if (!isatty(STDIN_FILENO)) {
            std::string line;
            if (std::getline(std::cin, line)) {
                std::stringstream ss(line);
                ss >> area_code >> country;
            }
        } else {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--area-code" && i + 1 < argc) {
                    area_code = argv[++i];
                } else if (arg == "--country" && i + 1 < argc) {
                    country = argv[++i];
                }
            }
        }

        if (!area_code.empty()) {
            std::cout << "Area Code: " << area_code << ", Country: " << country << "\n";
            return EXIT_SUCCESS;
        }
    }

    // Scenario 3: Real Interactive TUI Setup
    try {
        auto screen = ftxui::ScreenInteractive::Fullscreen();

        AppState state;
        
        LocationRepository repo(kDatabaseName);
        DatabaseController db_controller(repo, [&screen] {
            screen.PostEvent(ftxui::Event::Custom);
        });
        db_controller.InitializeDatabase();
        db_controller.LoadSavedLocations();

        LocationController loc_controller(state, db_controller, [&screen] {
            screen.PostEvent(ftxui::Event::Custom);
        });

        AboutState about_state;
        AboutController about_controller(about_state, [&screen] {
            screen.PostEvent(ftxui::Event::Custom);
        });

        AppController controller(state, loc_controller, about_controller, db_controller, screen.ExitLoopClosure());

        App app(state, controller);

        screen.Loop(app.GetComponent());
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Fatal TUI Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}

