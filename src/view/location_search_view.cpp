#include "view/location_search_view.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <mutex>

using namespace ftxui;

namespace weather_cli {

LocationSearchView::LocationSearchView(LocationController& controller)
    : controller_(controller) {

    auto& search_state = controller_.GetSearchState();

    // 1. Search input — styled via transform (bg applied here, not on the
    //    rendered element, so FTXUI's internal focusCursorBarBlinking can
    //    still place the terminal cursor correctly).
    InputOption search_input_option = InputOption::Default();
    search_input_option.content = &search_state.search_query;
    search_input_option.placeholder = "Enter city name to search";
    search_input_option.cursor_position = &search_state.cursor_position;
    search_input_option.multiline = false;
    search_input_option.insert = false;  // Block cursor (inverts colors) — always visible.
                                         // insert=true uses the terminal's native bar cursor
                                         // which is invisible on a black background.
    search_input_option.on_change = [this] {
        controller_.Search(controller_.GetSearchState().search_query);
    };
    search_input_option.transform = [](InputState s) {
        if (s.is_placeholder) {
            s.element |= dim;
        }
        s.element |= (s.focused ? color(Color::Green) : color(Color::White));
        return s.element;
    };
    search_input_ = Input(search_input_option);

    // 2. Suggestions menu
    MenuOption suggestions_option = MenuOption::Vertical();
    suggestions_option.on_enter = [this] {
        controller_.SelectSuggestion(controller_.GetSearchState().selected_suggestion_index);
    };
    suggestions_menu_ = Menu(&suggestion_entries_, &search_state.selected_suggestion_index, suggestions_option);

    // 3. Save checkbox
    save_checkbox_ = Checkbox("Save location to database", &search_state.save_to_db);

    // Build container — Container::Vertical correctly overrides SetActiveChild
    // so TakeFocus() propagation works through it.
    auto search_container = Container::Vertical({
        search_input_,
        save_checkbox_,
        suggestions_menu_
    });

    // Apply navigation event handling directly on the container using |=.
    // This is the canonical FTXUI pattern (see examples/component/input.cpp
    // lines 36-41: input_phone_number |= CatchEvent(...)).
    // Crucially, this keeps search_container as the direct child of Renderer
    // below, preserving the SetActiveChild focus chain intact.
    search_container |= CatchEvent([this](Event event) -> bool {
        auto& state = controller_.GetSearchState();
        bool show_modal = false;
        {
            std::lock_guard<std::mutex> lock(state.mutex);
            show_modal = state.show_search_modal;
        }

        if (!show_modal) {
            return false;
        }

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

        if (event == Event::Tab || event == Event::ArrowDown) {
            if (search_input_->Focused()) {
                save_checkbox_->TakeFocus();
            } else if (save_checkbox_->Focused()) {
                bool suggestions_available = false;
                {
                    std::lock_guard<std::mutex> lock(state.mutex);
                    suggestions_available = !state.search_suggestions.empty();
                }
                if (suggestions_available) {
                    state.selected_suggestion_index = 0;
                    suggestions_menu_->TakeFocus();
                }
                // No suggestions: stay on checkbox
            } else if (suggestions_menu_->Focused()) {
                if (event == Event::Tab) {
                    search_input_->TakeFocus();
                } else {
                    // ArrowDown: move selection down, clamp at last item
                    size_t suggestions_count = 0;
                    {
                        std::lock_guard<std::mutex> lock(state.mutex);
                        suggestions_count = state.search_suggestions.size();
                    }
                    if (suggestions_count > 0 &&
                        static_cast<size_t>(state.selected_suggestion_index) < suggestions_count - 1) {
                        state.selected_suggestion_index++;
                    }
                    // At last item: do nothing (consume event)
                }
            }
            return true;
        }

        if (event == Event::TabReverse || event == Event::ArrowUp) {
            if (search_input_->Focused()) {
                // ArrowUp from input: stay on input
            } else if (save_checkbox_->Focused()) {
                search_input_->TakeFocus();
            } else if (suggestions_menu_->Focused()) {
                if (event == Event::TabReverse) {
                    search_input_->TakeFocus();
                } else {
                    if (state.selected_suggestion_index <= 0) {
                        state.selected_suggestion_index = 0;
                        save_checkbox_->TakeFocus();
                    } else {
                        state.selected_suggestion_index--;
                    }
                }
            }
            return true;
        }

        return false;
    });

    // Renderer(search_container, lambda) — search_container (Container::Vertical
    // with |= CatchEvent decorator) is the direct child. This is identical to
    // the official input.cpp and composition.cpp examples. No wrapper sits
    // between Renderer and the Container, so the focus chain is unbroken.
    view_component_ = Renderer(search_container, [this] {
        auto& state = controller_.GetSearchState();

        bool show_modal = false;
        bool is_loading = false;
        bool has_error = false;
        std::string error_message;
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

        return vbox({
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
                text("[Esc] Cancel  [Tab/Arrows] Cycle Focus") | dim,
                filler(),
                text("[Enter] Search / Select / Toggle") | dim
            })
        }) | size(WIDTH, EQUAL, 60) | border | color(Color::Cyan) | clear_under | center;
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
