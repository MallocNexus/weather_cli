# Design Specification: LocationSearchView

This document provides a detailed design specification for the `LocationSearchView` modal in the `weather-cli` application, highlighting its FTXUI component layout, event routing, and sequence flows.

---

## 1. Component Hierarchy

`LocationSearchView` is designed using FTXUI's functional component pattern. To maintain hierarchical MVC separation, the View does not manage state mutations or threads. It delegates events to the `LocationSearchController` and binds read-only fields from `LocationSearchState` under mutex synchronization.

```
view_component_ (Renderer)
 └── search_container (Container::Vertical | CatchEvent decorator)
      ├── search_input_ (ftxui::Input)
      ├── search_button_ (ftxui::Button)
      ├── country_dropdown_ (ftxui::Dropdown wrapped in OnAfterEvent decorator)
      ├── save_checkbox_ (ftxui::Checkbox)
      └── suggestions_menu_ (ftxui::Menu)
```

---

## 2. Key Sequences & Flows

### A. Initialization Sequence

On application startup, views and controllers are wired hierarchically. The `LocationSearchView` receives a reference to `LocationSearchController` which itself holds a reference to `LocationController`.

```mermaid
sequenceDiagram
    autonumber
    participant Main as main.cpp
    participant LocCtrl as LocationController
    participant SearchCtrl as LocationSearchController
    participant AppCtrl as AppController
    participant View as LocationSearchView

    Main->>LocCtrl: Constructor(AppState, DatabaseController, RedrawCallback)
    Main->>SearchCtrl: Constructor(LocationController, RedrawCallback)
    Main->>AppCtrl: Constructor(AppState, LocationController, LocationSearchController, ...)
    Main->>View: Constructor(LocationSearchController)
    activate View
    View->>SearchCtrl: GetSearchState()
    SearchCtrl-->>View: LocationSearchState&
    deactivate View
```

---

### B. Keyboard Event & Focus Navigation Flow

The `search_container` intercepts keyboard events using a `CatchEvent` decorator to implement custom focus cycles and event routing:

```mermaid
sequenceDiagram
    autonumber
    participant User as Keyboard / User
    participant Container as search_container (CatchEvent)
    participant Input as search_input_
    participant Button as search_button_
    participant Dropdown as country_dropdown_
    participant Checkbox as save_checkbox_
    participant Menu as suggestions_menu_

    User->>Container: Press [Tab]
    alt search_input_ is focused
        Container->>Button: TakeFocus()
    else search_button_ is focused
        Container->>Dropdown: TakeFocus()
    else country_dropdown_ is focused
        Container->>Checkbox: TakeFocus()
    else save_checkbox_ is focused
        Container->>Menu: TakeFocus() (if suggestions exist)
    else suggestions_menu_ is focused
        Container->>Input: TakeFocus()
    end
```

---

### C. Search Invocation Sequence (Enter / Button Click)

When a search is explicitly triggered via a button click or pressing Enter inside the input box:

```mermaid
sequenceDiagram
    autonumber
    participant User as User / Keyboard
    participant View as LocationSearchView
    participant SearchCtrl as LocationSearchController
    participant State as LocationSearchState
    participant MainThread as TUI Main Thread
    participant BgThread as Worker Thread
    participant Service as GeocodingService

    User->>View: Press Search Button / [Enter]
    View->>SearchCtrl: TriggerSearch()
    activate SearchCtrl
    SearchCtrl->>State: Lock Mutex & read search_query
    State-->>SearchCtrl: query string
    SearchCtrl->>SearchCtrl: Search(query)
    SearchCtrl->>State: Lock Mutex & set is_loading = true, clear suggestions
    SearchCtrl->>MainThread: trigger_redraw_()
    MainThread->>View: Render "Searching..." loader state
    SearchCtrl->>BgThread: Spawn Worker Thread (detached)
    deactivate SearchCtrl
    
    activate BgThread
    BgThread->>Service: Search(query, country)
    activate Service
    Service-->>BgThread: std::vector<LocationMatch>
    deactivate Service
    BgThread->>State: Lock Mutex & compare search_id
    alt ID matches current_search_id_
        BgThread->>State: Save matches & set is_loading = false
        BgThread->>MainThread: trigger_redraw_()
        MainThread->>View: Render suggestions list
    end
    deactivate BgThread
```

---

### D. Country Filter Order-of-Operations (OnAfterEvent Decorator)

Because dropdown selection changes natively update the bound index *during* event execution, a simple `CatchEvent` decorator executes too early (capturing the old index value). To resolve this, a custom `OnAfterEvent` decorator delegates the event to the dropdown component *first*, and only fires when the dropdown returns `handled = true`:

```mermaid
sequenceDiagram
    autonumber
    participant User as Mouse Click / [Enter]
    participant Post as OnAfterEvent Wrapper
    participant Dropdown as country_dropdown_
    participant SearchCtrl as LocationSearchController
    participant State as LocationSearchState

    User->>Post: Dispatch event
    Post->>Dropdown: OnEvent(event) (natively processes)
    activate Dropdown
    Dropdown->>State: Mutate country_filter_index
    Dropdown-->>Post: returns handled = true
    deactivate Dropdown
    
    opt event is Return or Mouse-based
        Post->>SearchCtrl: SetCountryFilter(new_index)
        activate SearchCtrl
        SearchCtrl->>State: Lock Mutex & update country filter code
        SearchCtrl->>SearchCtrl: Search(active_query) (if query non-empty)
        deactivate SearchCtrl
    end
    Post-->>User: returns handled = true
```

---

### E. Decoupled Location Selection Flow

When the user selects a matching suggestion, coordinates updates are decoupled from the search modal context. The search controller delegates coordinates changes to `LocationController` and clears modal variables:

```mermaid
sequenceDiagram
    autonumber
    participant User as Click suggestion / press Enter
    participant View as LocationSearchView
    participant SearchCtrl as LocationSearchController
    participant LocCtrl as LocationController
    participant State as AppState
    participant DbCtrl as DatabaseController

    User->>View: Confirm selection
    View->>SearchCtrl: SelectSuggestion(index)
    activate SearchCtrl
    SearchCtrl->>LocCtrl: SelectLocation(match, save_to_db)
    activate LocCtrl
    LocCtrl->>State: Write coordinates (lat, lon, city_name)
    opt save_to_db is checked
        LocCtrl->>DbCtrl: SaveLocation(match)
    end
    LocCtrl->>LocCtrl: trigger_redraw_()
    deactivate LocCtrl
    SearchCtrl->>SearchCtrl: Reset search_suggestions & hide modal
    SearchCtrl->>SearchCtrl: trigger_redraw_()
    deactivate SearchCtrl
```

---

## 3. Thread Safety & Mutex Boundaries

Since TUI layout rendering takes place on the main execution thread, any concurrent writes performed by background worker threads (e.g. updating suggestions vectors or loading states) must be isolated to prevent concurrent modification data races.

* **Main Thread (Render Loop)**:
  At the beginning of each `view_component_` rendering pass, a short critical section locks `LocationSearchState::mutex`, copies required values (`show_search_modal`, `is_loading`, `has_error`, `error_message`, and `search_suggestions`), and immediately releases the lock.
* **Background Thread (HTTP Geocoder)**:
  Locks `LocationSearchState::mutex` before writing fetched geocoding results, resetting suggestion cursor coordinates, or modifying loading flags.
