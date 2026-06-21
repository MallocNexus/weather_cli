# Project Instructions for AI Agents

## Tech Stack
* Language: C++20
* UI Framework: FTXUI v6.1.9 (Functional Terminal (X) User Interface)
* Testing Framework: Catch2 v3.5.2
* Build System: CMake (with Ninja generator recommended)

## Important Commands
* **Configure:** `cmake -S . -B build -G Ninja` or `cmake -S . -B build`
* **Build:** `cmake --build build`
* **Run App:** `./build/weather_cli`
* **Run Tests:** `./build/run_tests`
* **Git Status:** `git status`
* **Git Diff:** `git diff`
* **Clang-format check:** `clang-format --dry-run --Werror src/**/*.cpp src/**/*.hpp tests/**/*.cpp`

## Architecture Notes
* The project follows a layered architecture split into subdirectories under `src/`:
  * **`model/`**: Domain Entities and state structures.
    * `AppState` (`src/model/app_state.hpp`): Shared data structure containing UI state.
    * `WeatherData` (`src/model/weather_data.hpp` / `src/model/weather_data.cpp`): Structures representation of forecasts.
  * **`service/`**: Domain Services (stateless computation, networking, and parser operations).
    * `HttpClient` (`src/service/http_client.hpp` / `src/service/http_client.cpp`): Networking requests via CPR.
    * `WeatherParser` (`src/service/weather_parser.hpp` / `src/service/weather_parser.cpp`): JSON parser logic.
  * **`controller/`**: Coordinator logic (processes triggers and updates models).
    * `AppController` (`src/controller/app_controller.hpp` / `src/controller/app_controller.cpp`).
  * **`view/`**: User interface components.
    * `App` (`src/view/app.hpp` / `src/view/app.cpp`): Layout grids & tabs.
    * `WeatherIcon` (`src/view/weather_icon.hpp` / `src/view/weather_icon.cpp`): ASCII weather codes icon formatter.
    * `SparklineGraph` (`src/view/sparkline_graph.hpp` / `src/view/sparkline_graph.cpp`): Custom ftxui canvas graphs drawing.
  * **`util/`**: Helpers, formatting, and constants.
    * `constants.hpp` (`src/util/constants.hpp`): Centralized parameters (default coordinates, API endpoint URLs).
    * `formatting.cpp` (`src/util/formatting.cpp`): Celsius-Fahrenheit translation converters, text formatting.
* SQL string construction in `model/` or `service/` uses `std::format` (requires C++20).

## Adding New Features / Modifying
* **Constants**: Extract API endpoints, default lat/long, symbols, ASCII models to `src/util/constants.hpp` rather than embedding them directly in functional headers or source files.
* **Tests**: Organized under `tests/` in matching subdirectories:
  * `tests/util/`
  * `tests/model/`
  * `tests/service/`
  * `tests/controller/`
  * `tests/view/`

## General Guidelines
* The [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) is the formatting authority. **Exception:** use 4-space indentation instead of 2.
* Private member variables use a **trailing underscore** (e.g., `state_`, `controller_`).
* `using namespace ftxui;` is permitted in `.cpp` files only — never in headers.
* `#pragma once` is used for all header guards.

## Approved Terminal Commands
The following commands are approved for execution during development:
* `cmake -S . -B build` and `cmake --build build`
* `git status` and `git diff`
* `./build/run_tests`
* `ls`, `find`, `file`, `cat`
