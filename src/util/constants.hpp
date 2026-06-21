#pragma once

#include <string>
#include <string_view>

namespace weather_cli {
// clang-format off
constexpr double kDefaultLatitude = -33.8688;
constexpr double kDefaultLongitude = 151.2093;
const std::string kDefaultCityName = "Sydney";

const std::string kApiEndpoint = "https://api.open-meteo.com/v1/forecast";
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
}  // namespace weather_cli
