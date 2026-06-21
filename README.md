# Quick Weather CLI App

A fast, low-latency forecast utility pulling data via a lightweight API (Open-Meteo), featuring:
- **FTXUI TUI Components**: Canvas for drawing sparkline temperature graphs or rain probability charts, ASCII art renderers for weather status icons, and an interactive Slider to scrub through the hourly forecast timeline.
- **Asynchronous C++ HTTP client**: Background requests fetched via CPR (C++ Requests) to ensure the render thread is non-blocking.
- **Offline cache**: SQLite3 storage backend for low-latency retrieval.
- **MVC + DDD Architecture**: Strict layer isolation (View $\rightarrow$ Controller $\rightarrow$ Model/Service).

## Build Requirements
- C++20 compliant compiler (GCC 10+, Clang 10+, or MSVC 2019+)
- CMake (3.14+)
- SQLite3 development headers and library

## Quick Start
```bash
# Configure the build directory
cmake -S . -B build -G Ninja

# Compile the target executable
cmake --build build

# Launch the interactive terminal UI
./build/weather_cli

# Run unit tests
./build/run_tests
```
