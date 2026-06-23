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
#include "controller/forecast_controller.hpp"
#include "service/geocoding_service.hpp"
#include "view/app.hpp"
#include "util/constants.hpp"

int main(int argc, char* argv[]) {
    using namespace weather_cli;

    std::string query = "";
    std::string country = "";
    bool is_city = false;

    // Scenario 1 & 2: Headless arguments or Pipe/Stdin input handling
    if (argc >= 2 || !isatty(STDIN_FILENO)) {
        if (!isatty(STDIN_FILENO)) {
            std::string line;
            if (std::getline(std::cin, line)) {
                std::stringstream ss(line);
                ss >> query >> country;
            }
        } else {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if ((arg == "--area-code" || arg == "--city") && i + 1 < argc) {
                    query = argv[++i];
                    if (arg == "--city") {
                        is_city = true;
                    }
                } else if (arg == "--country" && i + 1 < argc) {
                    country = argv[++i];
                }
            }
        }

        if (!query.empty()) {
            auto matches = GeocodingService::Search(query, country);
            if (matches.empty()) {
                std::cerr << "Error: No location found for " << (is_city ? "city" : "area code") << " '" << query << "'";
                if (!country.empty()) std::cerr << " and country '" << country << "'";
                std::cerr << ".\n";
                return EXIT_FAILURE;
            }

            const auto& loc = matches.front();
            std::cout << "Resolved location:\n"
                      << "  City:    " << loc.name << "\n"
                      << "  Region:  " << loc.region << "\n"
                      << "  Country: " << loc.country << "\n"
                      << "  Lat:     " << loc.latitude << "\n"
                      << "  Lon:     " << loc.longitude << "\n";
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

        ForecastController forecast_controller(state, [&screen] {
            screen.PostEvent(ftxui::Event::Custom);
        });

        AppController controller(state, loc_controller, about_controller, db_controller, forecast_controller, screen.ExitLoopClosure());

        App app(state, controller);

        // Trigger the initial weather fetch before entering the render loop.
        forecast_controller.Refresh();

        screen.Loop(app.GetComponent());
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Fatal TUI Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}

