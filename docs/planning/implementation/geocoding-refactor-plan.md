# Geocoding Service Refactor Plan

**Goal:** Extract the Zippopotam.us fallback logic from `GeocodingService` into its own dedicated `ZippopotamService` class to ensure separation of concerns and adhere to the Single Responsibility Principle. `GeocodingService` will act as the primary router, delegating numeric postcode queries to `ZippopotamService` and routing standard city queries to the Open-Meteo API.

## Phase 1 — Create ZippopotamService
- [x] **Step 1.1** — Create `src/service/zippopotam_service.hpp`:
  - [x] Define the `ZippopotamService` class within the `weather_cli` namespace.
  - [x] Declare a public `static std::vector<LocationMatch> Search(const std::string& postcode, const std::string& country_code);` method.
  - [x] Declare a public `static std::vector<LocationMatch> ParseResponse(const std::string& json_str);` method (renamed from `ParseZippopotamResponse`).
- [x] **Step 1.2** — Create `src/service/zippopotam_service.cpp`:
  - [x] Move the `to_lower()` helper utility into this file's anonymous namespace.
  - [x] Move the specific Zippopotam HTTP fetch logic (`https://api.zippopotam.us/...`) into `ZippopotamService::Search()`.
  - [x] Move the `ParseZippopotamResponse` implementation from `geocoding_service.cpp` to `ZippopotamService::ParseResponse()`.

## Phase 2 — Refactor GeocodingService (The Facade)
- [x] **Step 2.1** — Update `src/service/geocoding_service.hpp`:
  - [x] Remove the `ParseZippopotamResponse` declaration.
- [x] **Step 2.2** — Update `src/service/geocoding_service.cpp`:
  - [x] Include `"service/zippopotam_service.hpp"`.
  - [x] Keep `is_numeric()` utility here to determine routing.
  - [x] Modify `GeocodingService::Search` to act as a router: if the query is numeric and has a country code, delegate to `ZippopotamService::Search(query, country_code)`.
  - [x] Remove the unused Zippopotam parsing logic and string helper utilities from this file.

## Phase 3 — CMake Integration
- [x] **Step 3.1** — Update `CMakeLists.txt`:
  - [x] Add `src/service/zippopotam_service.cpp` to the `weather_lib` target sources.

## Phase 4 — Test Extraction & Verification
- [x] **Step 4.1** — Extract Zippopotam Tests:
  - [x] Create `tests/service/test_zippopotam_service.cpp`.
  - [x] Move the `"Parses valid Zippopotam JSON response correctly"` Catch2 section from `test_geocoding_service.cpp` into this new test file.
- [x] **Step 4.2** — Update `CMakeLists.txt` for Tests:
  - [x] Add `tests/service/test_zippopotam_service.cpp` to the `run_tests` executable target.
- [x] **Step 4.3** — Build and Test:
  - [x] Run `cmake -S . -B build && cmake --build build`.
  - [x] Run `./build/run_tests` to ensure both Open-Meteo and Zippopotam tests pass.
  - [x] Manual verification: `./build/weather_cli --area-code 2153 --country AU` should still successfully return Baulkham Hills.
