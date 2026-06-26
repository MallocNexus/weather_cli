#include "view/app.hpp"
#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/component.hpp>
#include "util/formatting.hpp"
#include "util/constants.hpp"
#include "controller/db_controller.hpp"

using namespace ftxui;

namespace weather_cli {

App::App(AppState& state, AppController& controller)
    : state_(state), controller_(controller),
      location_search_view_(controller.GetLocationSearchController()),
      about_view_(controller.GetAboutController()) {

    // 1. Top animated navigation tab bar
    auto top_menu = Menu(&top_menu_entries_, &top_menu_selected_, MenuOption::HorizontalAnimated());

    // 2. Tab options menus
    MenuOption app_option = MenuOption::HorizontalAnimated();
    app_option.on_enter = [this] {
        if (app_selected_ == 0) {
            controller_.RefreshForecast();
        } else if (app_selected_ == 1) {
            controller_.Quit();
        }
    };
    auto app_menu = Menu(&app_entries_, &app_selected_, app_option);

    MenuOption locations_option = MenuOption::HorizontalAnimated();
    locations_option.on_enter = [this] {
        if (locations_selected_ == 0) {
            controller_.OpenSearch();
        } else {
            controller_.SelectSavedLocation(locations_selected_ - 1);
        }
    };
    auto locations_menu = Menu(&locations_entries_, &locations_selected_, locations_option);

    MenuOption units_option = MenuOption::HorizontalAnimated();
    units_option.on_enter = [this] {
        if (units_selected_ == 0) {
            controller_.ToggleUnits();
            if (!state_.is_celsius) controller_.ToggleUnits(); // Force C
        } else if (units_selected_ == 1) {
            controller_.ToggleUnits();
            if (state_.is_celsius) controller_.ToggleUnits();  // Force F
        }
    };
    auto units_menu = Menu(&units_entries_, &units_selected_, units_option);

    MenuOption help_option = MenuOption::HorizontalAnimated();
    help_option.on_enter = [this] {
        if (help_selected_ == 0) {
            controller_.OpenAbout();
        }
    };
    auto help_menu = Menu(&help_entries_, &help_selected_, help_option);

    auto tab_container = Container::Tab(
        {app_menu, locations_menu, units_menu, help_menu},
        &top_menu_selected_
    );

    // 3. View selectors tabs for the trend graph
    auto graph_tabs = Toggle(&graph_tab_entries_, &graph_tab_selected_);

    // 4. Interactive Timeline scrub slider
    auto timeline_slider = Slider("Timeline Offset: ", &state_.selected_hour_index, 0, 23, 1);

    // Layout focus containment
    auto main_container = Container::Vertical({
        top_menu,
        tab_container,
        graph_tabs,
        timeline_slider
    });

    auto root_container = Container::Tab(
        {main_container, location_search_view_.GetComponent(), about_view_.GetComponent()},
        &root_tab_selected_
    );

    // Keep root_tab_selected_ in sync BEFORE events are routed.
    // ContainerBase::OnEvent reads the selector during event processing
    // (before render runs), so updating it only in the Renderer lambda was
    // too late — the Tab always forwarded events to tab 0 (main), meaning
    // search_input_ was never in the focused subtree and Focused() returned false.
    auto root_with_sync = CatchEvent(root_container, [this](Event /*event*/) {
        root_tab_selected_ = controller_.IsSearchModalOpen() ? 1
                           : controller_.IsAboutModalOpen()  ? 2
                                                             : 0;
        return false;  // never consume — just sync the selector then pass through
    });

    main_renderer_ = Renderer(root_with_sync, [&, top_menu, tab_container, graph_tabs, timeline_slider] {
        // Rebuild locations menu items dynamically from database repository favorites
        const auto& saved_locs = controller_.GetDatabaseController().GetRepository().GetSavedLocations();
        if (locations_entries_.size() != saved_locs.size() + 1) {
            locations_entries_ = {"Search Location"};
            for (const auto& loc : saved_locs) {
                std::string label = loc.name;
                if (!loc.country.empty()) {
                    label += " (" + loc.country + ")";
                }
                locations_entries_.push_back(label);
            }
        }

        // Fetch show_search_modal and show_about_modal state to coordinate active tab
        bool show_search_modal = controller_.IsSearchModalOpen();
        bool show_about_modal = controller_.IsAboutModalOpen();

        if (show_search_modal) {
            root_tab_selected_ = 1;
        } else if (show_about_modal) {
            root_tab_selected_ = 2;
        } else {
            root_tab_selected_ = 0;
        }

        // Unpack the optional once — cc is null while loading or on error.
        const CurrentConditions* cc =
            state_.current_conditions.has_value()
                ? &state_.current_conditions.value()
                : nullptr;

        const double current_temp = cc ? cc->temperature : 0.0;
        const double max_temp     = cc ? cc->daily_max   : 0.0;
        const double min_temp     = cc ? cc->daily_min   : 0.0;
        const int    humidity     = cc ? cc->humidity    : 0;
        const double wind_speed_v = cc ? cc->wind_speed  : 0.0;
        const int    wmo_code     = cc ? cc->weather_code : 0;
        const int    is_day       = cc ? cc->is_day       : 1;

        // Dynamic detail metrics calculator based on slider scrubbing
        int selected_hour = state_.selected_hour_index;
        double hour_temp  = 15.0 + (selected_hour % 7);
        int rain_prob     = (selected_hour * 4) % 100;
        // Hourly wind stays mocked until Phase 16.2 wires real hourly data.
        int wind_speed_hourly = 8 + (selected_hour * 2) % 20;

        // ASCII icon — driven by live WMO code + day/night via WeatherIcon.
        Elements icon_lines;
        for (const auto& line : WeatherIcon::GetIcon(wmo_code, is_day)) {
            icon_lines.push_back(text(line) | color(Color::BlueLight));
        }
        auto weather_icon_element = vbox(std::move(icon_lines));

        // Condition string — live WMO description, or loading/error fallback.
        std::string condition_str;
        if (state_.is_loading) {
            condition_str = "Loading...";
        } else if (state_.has_error) {
            condition_str = "Error: " + state_.error_message;
        } else {
            condition_str = WeatherIcon::GetDescription(wmo_code);
        }

        // Weather text metrics summary
        auto summary_panel = hbox({
            weather_icon_element,
            separator(),
            vbox({
                text("Current Temperature: " + FormatTemperature(current_temp, state_.is_celsius)) | bold,
                text("Condition: " + condition_str),
                text("Wind: " + std::to_string(static_cast<int>(wind_speed_v)) + (state_.is_celsius ? " km/h" : " mph") + "  Humidity: " + std::to_string(humidity) + "%"),
                text("Daily Max: " + FormatTemperature(max_temp, state_.is_celsius) + "  Min: " + FormatTemperature(min_temp, state_.is_celsius)),
            }) | size(WIDTH, GREATER_THAN, 40)
        });

        // Canvas plotter mock
        auto c = Canvas(140, 24);
        // Draw horizontal axis border
        for (int x = 6; x < 140; ++x) {
            c.DrawPoint(x, 18, true);
        }
        // Draw vertical axis border
        for (int y = 0; y < 24; ++y) {
            c.DrawPoint(6, y, true);
        }
        // Draw mock curve to check canvas display
        for (int x = 6; x < 140; ++x) {
            double rad = (x - 6) / 20.0;
            int y = 10 + static_cast<int>(5.0 * std::sin(rad));
            c.DrawPoint(x, y, true);
        }

        auto canvas_element = vbox({
            hbox({
                vbox({
                    text(graph_tab_selected_ == 0 ? "24°C" : "100%"),
                    filler(),
                    text(graph_tab_selected_ == 0 ? "16°C" : "50%"),
                    filler(),
                    text(graph_tab_selected_ == 0 ? "8°C" : "0%"),
                    filler(),
                }) | size(WIDTH, EQUAL, 6),
                canvas(std::move(c)) | border
            }),
            hbox({
                text("       00:00        06:00        12:00        18:00")
            })
        });

        // Assemble main application frame layout
        Element document = vbox({
            vbox({
                top_menu->Render(),
                separator(),
                tab_container->Render(),
            }) | border,
            hbox({
                text(state_.city_name + " (Active)") | bold | color(Color::Green),
                filler(),
                text("Lat: " + std::to_string(state_.latitude) + "  Lon: " + std::to_string(state_.longitude)) | dim
            }) | border,
            summary_panel | border,
            vbox({
                graph_tabs->Render(),
                separator(),
                canvas_element
            }) | border,
            vbox({
                timeline_slider->Render(),
                separator(),
                text(std::string("Selected Hour Summary (") + (selected_hour < 10 ? "0" : "") + std::to_string(selected_hour) + ":00):") | bold,
                text("  Temp: " + FormatTemperature(hour_temp, state_.is_celsius) + 
                     "  Rain Probability: " + std::to_string(rain_prob) + "%" +
                     "  Wind: " + std::to_string(wind_speed_hourly) + (state_.is_celsius ? " km/h" : " mph") +
                     "  Condition: " + condition_str)
            }) | border
        });

        // Layer the modal search and about views if active
        document = location_search_view_.Render(document);
        document = about_view_.Render(document);

        return document;
    });
}

Component App::GetComponent() {
    return main_renderer_;
}

}  // namespace weather_cli
