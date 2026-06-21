cmake_minimum_required(VERSION 3.14)
project(WeatherCli VERSION 1.0.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

include(FetchContent)

# Fetch FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG        v6.1.9
)
FetchContent_MakeAvailable(ftxui)

# Fetch Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Fetch CPR (C++ Requests)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.10.4
)
set(CPR_BUILD_TESTS OFF CACHE INTERNAL "")
set(CPR_USE_SYSTEM_CURL ON CACHE INTERNAL "")
FetchContent_MakeAvailable(cpr)

# Fetch nlohmann/json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

find_package(SQLite3 REQUIRED)

# ---------------------------------------------------------------------------
# Utility library
# ---------------------------------------------------------------------------
add_library(util_lib STATIC src/util/formatting.cpp)
target_include_directories(util_lib PUBLIC src/)

# ---------------------------------------------------------------------------
# Layer 1: Model & Services (Geocoding implementation)
# ---------------------------------------------------------------------------
add_library(weather_lib STATIC
    src/service/http_client.cpp
    src/service/geocoding_service.cpp
)
target_include_directories(weather_lib PUBLIC src/)
target_link_libraries(weather_lib PUBLIC
    util_lib
    cpr::cpr
    nlohmann_json::nlohmann_json
    SQLite3::SQLite3
)

# ---------------------------------------------------------------------------
# Layer 2: Controller
# ---------------------------------------------------------------------------
add_library(controller_lib STATIC 
    src/controller/app_controller.cpp
    src/controller/location_controller.cpp
)
target_include_directories(controller_lib PUBLIC src/)
target_link_libraries(controller_lib PUBLIC weather_lib util_lib)

# ---------------------------------------------------------------------------
# Layer 3: View
# ---------------------------------------------------------------------------
add_library(app_lib STATIC
    src/view/app.cpp
    src/view/location_search_view.cpp
)
target_include_directories(app_lib PUBLIC src/)
target_link_libraries(app_lib PUBLIC
    ftxui::screen
    ftxui::dom
    ftxui::component
    controller_lib
)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(weather_cli src/main.cpp)
target_link_libraries(weather_cli PRIVATE app_lib)

# ---------------------------------------------------------------------------
# Test executable (Enabled in Phase 5)
# ---------------------------------------------------------------------------
add_executable(run_tests
    tests/test_main.cpp
    tests/service/test_http_client.cpp
    tests/service/test_geocoding_service.cpp
    tests/controller/test_location_controller.cpp
    tests/controller/test_app_controller.cpp
    tests/util/test_formatting.cpp
    tests/view/test_app.cpp
)
target_include_directories(run_tests PUBLIC src/)
target_link_libraries(run_tests PRIVATE
    Catch2::Catch2WithMain
    app_lib
    controller_lib
    weather_lib
    util_lib
)
