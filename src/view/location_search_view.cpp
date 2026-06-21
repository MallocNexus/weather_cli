#include "view/location_search_view.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <mutex>

using namespace ftxui;

namespace weather_cli {

LocationSearchView::LocationSearchView(LocationController& controller)
    : controller_(controller) {

    auto& search_state = controller_.GetSearchState();

    // 1. Geocoding Search input
    InputOption search_input_option = InputOption::Default();
    search_input_option.content = &search_state.search_query;
    search_input_option.placeholder = "Enter city name to search";
    search_input_option.multiline = false;
    search_input_option.on_change = [this] {
        controller_.Search(controller_.GetSearchState().search_query);
    };
    search_input_ = Input(search_input_option);

    // 2. Geocoding suggestions menu list
    MenuOption suggestions_option = MenuOption::Vertical();
    suggestions_option.on_enter = [this] {
        controller_.SelectSuggestion(controller_.GetSearchState().selected_suggestion_index);
    };
    suggestions_menu_ = Menu(&suggestion_entries_, &search_state.selected_suggestion_index, suggestions_option);

    // 3. Save checkbox option
    save_checkbox_ = Checkbox("Save location to database", &search_state.save_to_db);

    // Layout focus containment
    auto search_container = Container::Vertical({
        search_input_,
        suggestions_menu_,
        save_checkbox_
    });

    // Intercept Escape to close modal, and Enter to shift focus to suggestions
    auto event_dispatcher = CatchEvent(search_container, [this](Event event) {
        auto& state = controller_.GetSearchState();
        bool show_modal = false;
        {
            std::lock_guard<std::mutex> lock(state.mutex);
            show_modal = state.show_search_modal;
        }

        if (show_modal) {
            if (event == Event::Escape) {
                controller_.CancelSearch();
                return true;
            }
            if (event == Event::Return) {
                if (search_input_->Focused()) {
                    bool suggestions_available = false;
                    {
                        std::lock_guard<std::mutex> lock(state.mutex);
                        suggestions_available = !state.search_suggestions.empty();
                    }
                    if (suggestions_available) {
                        suggestions_menu_->TakeFocus();
                    }
                    return true;
                }
            }
        }
        return false;
    });

    // Wrap in a custom renderer that manages focus transitions and populates labels dynamically
    view_component_ = Renderer(event_dispatcher, [this] {
        auto& state = controller_.GetSearchState();
        
        bool show_modal = false;
        bool is_loading = false;
        bool has_error = false;
        std::string error_message = "";
        std::vector<LocationMatch> suggestions_copy;
        {
            std::lock_guard<std::mutex> lock(state.mutex);
            show_modal = state.show_search_modal;
            is_loading = state.is_loading;
            has_error = state.has_error;
            error_message = state.error_message;
            suggestions_copy = state.search_suggestions;
        }

        if (show_modal && !last_show_search_modal_) {
            search_input_->TakeFocus();
        }
        last_show_search_modal_ = show_modal;

        // Populate suggestion labels dynamically
        suggestion_entries_.clear();
        for (const auto& match : suggestions_copy) {
            std::string label = match.name;
            if (!match.region.empty()) {
                label += ", " + match.region;
            }
            if (!match.country.empty()) {
                label += " (" + match.country + ")";
            }
            suggestion_entries_.push_back(label);
        }

        auto suggestions_box = suggestion_entries_.empty()
            ? (is_loading ? text("Searching...")
                          : (has_error ? (text("Error: " + error_message) | color(Color::Red))
                                       : text("No matches found")))
            : (suggestions_menu_->Render() | vscroll_indicator | frame);

        auto modal_element = vbox({
            text("Search Location") | bold | center,
            separator(),
            text("Enter city name and press Enter to query:") | dim,
            search_input_->Render() | border,
            separator(),
            save_checkbox_->Render() | border,
            separator(),
            text("Matching Cities:") | bold,
            suggestions_box | size(HEIGHT, GREATER_THAN, 5) | border,
            separator(),
            hbox({
                text("[Esc] Cancel") | dim,
                filler(),
                text("[Enter] Search / Select / Toggle") | dim
            })
        }) | size(WIDTH, EQUAL, 60) | border | color(Color::Cyan) | clear_under | center;

        return modal_element;
    });
}

Component LocationSearchView::GetComponent() {
    return view_component_;
}

Element LocationSearchView::Render(Element base_document) {
    auto& state = controller_.GetSearchState();
    bool show_modal = false;
    {
        std::lock_guard<std::mutex> lock(state.mutex);
        show_modal = state.show_search_modal;
    }

    if (!show_modal) {
        return base_document;
    }

    return dbox({
        base_document,
        view_component_->Render()
    });
}

}  // namespace weather_cli
