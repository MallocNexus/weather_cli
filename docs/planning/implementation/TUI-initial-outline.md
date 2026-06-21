# Quick Weather CLI App TUI Specifications

This document defines the layout, visual styling, components, and interaction patterns of the `weather-cli` Terminal User Interface (TUI), using the `calc-cli` interface design as a baseline.

---

## 1. General Layout

The interface is enclosed in a single outer border and partitioned by horizontal separator lines into six key sections. It fits dynamically within the terminal window.

```text
┌────────────────────────────────────────────────────────────┐
│ App  Locations  Units  Help                                │ <- Top Menu Bar
├────────────────────────────────────────────────────────────┤
│ Sydney, AU (Active)  |  Lat: -33.87  Lon: 151.21           │ <- Status Banner
├────────────────────────────────────────────────────────────┤
│    .------.      Current Temperature: 18.5°C               │
│   /  /  / |      Condition: Light Rain                     │ <- Current Weather Summary
│    '------'      Wind: 14 km/h  Humidity: 85%              │    (with ASCII Art Icon)
│   /  /  /        High: 21.0°C   Low: 12.5°C                │
├────────────────────────────────────────────────────────────┤
│ [Hourly Temperature Graph]  Hourly Rain Probability        │ <- Toggle view tabs
├────────────────────────────────────────────────────────────┤
│ 24°C ───  /\                                               │
│ 20°C ──  /  \    /\                                        │ <- FTXUI Canvas Plotting Area
│ 16°C ─_ /    \__/  \                                       │
│ 12°C ─             \______                                 │
│        00:00   06:00   12:00   18:00                       │
├────────────────────────────────────────────────────────────┤
│ Timeline Scrubber:                                         │
│ [ 09:00 ] ──────■───────────────────────────────           │ <- Scrubber Slider (Interactive)
├────────────────────────────────────────────────────────────┤
│ Selected Hour Summary (09:00):                             │
│   Temp: 16.2°C  Rain Prob: 60%  Wind: 12 km/h  WMO: Rain   │ <- Selected hour details panel
└────────────────────────────────────────────────────────────┘
```

---

## 2. Component Design & Behaviors

### A. Top Menu Bar & Options
* **App**:
  - `Refresh`: Re-fetches weather data asynchronously (spawns background thread, displays loading animation).
  - `Quit`: Closes the application.
* **Locations**:
  - `Search Location`: Opens the Search Modal to look up cities.
  - `Sydney, AU`: Shortcut to quickly swap coordinates to Sydney.
  - `New York, US`: Shortcut to quickly swap coordinates to New York.
* **Units**:
  - `Celsius (°C)`: Sets temperature to Celsius and wind speed to km/h.
  - `Fahrenheit (°F)`: Sets temperature to Fahrenheit and wind speed to mph.
* **Help**:
  - `Version`: Opens the Version Modal.
  - `Shortcuts`: Opens the Keyboard Shortcuts Reference Modal.

### B. Status Banner & Current Weather Summary
* Displays the active location name and coordinates.
* **Weather Icon (ASCII)**: Renders a WMO condition icon based on the active forecast:
  - *Sunny*: Sun symbol with rays.
  - *Cloudy*: Standard horizontal cloud layers.
  - *Rainy*: Cloud outline with angled falling drop dashes (`/ /`).
  - *Stormy*: Cloud outline with thunderbolt symbol (`⚡`).
  - *Snowy*: Cloud outline with snowflake asterisks (`* *`).
* **Condition Text**: Human-readable translation of the WMO status code (e.g. "Mainly clear", "Violent rain showers").
* **Temperature & Metrics**: Scaled based on the unit settings. Max/Min bounds represent the daily extremes.

### C. Hourly Trend Graph (FTXUI Canvas)
* **Tabs Options**: Horizontal selection tabs allow toggling between the **Temperature Graph** and **Rain Probability Graph**.
* **Visual Graph**: Drawn dynamically using `ftxui::Canvas` and line plotting functions (`DrawPoint`, `DrawLine`). 
* **Axes**: 
  - Y-Axis: Automatically adjusted depending on the peak value ranges (e.g., peak rain chance: 0% to 100%; temperature range: daily low to daily high).
  - X-Axis: 24-hour horizontal ticks (`00:00`, `06:00`, `12:00`, `18:00`).

### D. Timeline Scrubber (ftxui::Slider)
* **Slider Component**: Maps to index offsets of the forecast timeline. In standard TUI, it operates over the 24 hours of the active forecast day (or 0-167 for the full weekly series).
* **Scrub Behavior**: Modifying the slider updates `selected_hour_index` in `AppState`.
* **Details Update**: The details panel underneath displays the weather metrics specific to that selected timeline hour.

---

## 3. Keyboard Shortcuts & Event Handling

Keyboard hooks allow quick adjustments without browsing menus:
* `Tab` / `Shift-Tab`: Shift focus between panels (Top Menu $\rightarrow$ Graph Selector $\rightarrow$ Timeline Slider).
* `Right-Arrow` / `Left-Arrow` (when Slider focused): Scrubs the hourly scrubber back/forth.
* `R` / `r`: Instantly triggers forecast refresh.
* `U` / `u`: Toggles between metric (`°C`) and imperial (`°F`) units.
* `S` / `s`: Instantly opens the Search Location Modal.
* `Q` / `q` / `Esc`: Quits the application.

---

## 4. Dialog Modals

### A. Location Search Modal
```text
┌──────────────────────────────────────────┐
│             Search Location              │
├──────────────────────────────────────────┤
│ Search: [ Lon_                     ]     │ <- Input field (active text editor)
├──────────────────────────────────────────┤
│ Suggestions:                             │
│   > London, GB  (Lat: 51.50, Lon: -0.12)  │ <- Suggestion list navigation (Vertical Menu)
│     London, CA  (Lat: 42.98, Lon: -81.24)│
│     Long Beach, US (Lat: 33.77, Lon: -118)│
├──────────────────────────────────────────┤
│           [ OK ]     [ Cancel ]          │
└──────────────────────────────────────────┘
```
* **Trigger**: Activated when the Locations $\rightarrow$ Search menu is selected or the `S` key is pressed.
* **Behavior**: Typing queries immediately queries geocoding data in the background (or parses local SQLite database for city matches). Pressing `OK` or choosing a suggestion shifts coordinates and updates forecasts.

### B. Keyboard Shortcuts Reference Modal
```text
┌──────────────────────────────────────────┐
│            Keyboard Shortcuts            │
├──────────────────────────────────────────┤
│  [Tab]         Cycle Panel Focus         │
│  [Arrows]      Scrub Timeline Hours      │
│  [R] / [r]     Refresh Weather Forecast  │
│  [U] / [u]     Toggle Temperature Units  │
│  [S] / [s]     Open City Search Modal    │
│  [Q] / [Esc]   Quit Application          │
├──────────────────────────────────────────┤
│                [ Close ]                 │
└──────────────────────────────────────────┘
```
* **Trigger**: Activated when Help $\rightarrow$ Shortcuts is selected. Shows a read-only list of keys.
