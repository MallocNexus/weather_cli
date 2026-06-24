#pragma once

#include <array>
#include <string>
#include <string_view>

namespace weather_cli {
// clang-format off
constexpr double kDefaultLatitude = -33.8688;
constexpr double kDefaultLongitude = 151.2093;
const std::string kDefaultCityName = "Sydney";

// Open-Meteo Forecast API
constexpr std::string_view kForecastApiEndpoint =
    "https://api.open-meteo.com/v1/forecast";
constexpr std::string_view kCurrentParams =
    "temperature_2m,relative_humidity_2m,wind_speed_10m,"
    "weather_code,apparent_temperature,is_day";
constexpr std::string_view kDailyParams =
    "temperature_2m_max,temperature_2m_min";
constexpr std::string_view kForecastTimeZone = "auto";
const std::string kDatabaseName = "weather_cache.db";
constexpr int kCacheTtlSeconds = 1800;

const std::string kAppName = "Weather CLI";
const std::string kAppVersion = "0.1.0";

// Database Schema Constants
constexpr std::string_view kDbLocationTable = "saved_locations";
constexpr std::string_view kDbLocationColName = "name";
constexpr std::string_view kDbLocationColCountry = "country";
constexpr std::string_view kDbLocationColRegion = "region";
constexpr std::string_view kDbLocationColLatitude = "latitude";
constexpr std::string_view kDbLocationColLongitude = "longitude";
// clang-format on

// ---------------------------------------------------------------------------
// Country filter list for the location search modal.
// Index 0 must be "AU" so the default country_filter_index = 0 maps to
// Australia. The final "Any Country" entry uses an empty code — forwarding
// it to GeocodingService::Search applies no country restriction.
// ---------------------------------------------------------------------------
struct CountryEntry {
    std::string_view label;  // Human-readable display name shown in the dropdown.
    std::string_view code;   // ISO 3166-1 alpha-2 code (empty = no filter).
};

constexpr std::array<CountryEntry, 11> kCountryList = {{
    {"Australia",      "AU"},
    {"United States",  "US"},
    {"United Kingdom", "GB"},
    {"Canada",         "CA"},
    {"New Zealand",    "NZ"},
    {"Germany",        "DE"},
    {"France",         "FR"},
    {"Japan",          "JP"},
    {"India",          "IN"},
    {"Brazil",         "BR"},
    {"Any Country",    ""},   // empty → GeocodingService uses no country filter
}};

}  // namespace weather_cli
