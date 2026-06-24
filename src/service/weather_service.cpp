#include "service/weather_service.hpp"
#include "service/http_client.hpp"
#include "util/constants.hpp"
#include <nlohmann/json.hpp>
#include <format>
#include <stdexcept>

namespace weather_cli {

// ---------------------------------------------------------------------------
// URL construction
// ---------------------------------------------------------------------------

// Builds the Open-Meteo current-conditions URL for the given coordinates.
// Example output:
//   https://api.open-meteo.com/v1/forecast
//     ?latitude=-33.8688&longitude=151.2093
//     &current=temperature_2m,relative_humidity_2m,wind_speed_10m,
//              weather_code,apparent_temperature
//     &daily=temperature_2m_max,temperature_2m_min
//     &timezone=auto
//     &forecast_days=1
static std::string BuildUrl(double latitude, double longitude) {
    return std::format(
        "{}?latitude={:.4f}&longitude={:.4f}"
        "&current={}"
        "&daily={}"
        "&timezone={}"
        "&forecast_days=1",
        kForecastApiEndpoint,
        latitude,
        longitude,
        kCurrentParams,
        kDailyParams,
        kForecastTimeZone);
}

// ---------------------------------------------------------------------------
// WeatherService implementation
// ---------------------------------------------------------------------------

CurrentConditions WeatherService::ParseCurrentConditions(
    const std::string& json_str) {
    using json = nlohmann::json;

    const json root = json::parse(json_str);

    const json& current = root.at("current");
    const json& daily   = root.at("daily");

    CurrentConditions cc;
    cc.temperature  = current.at("temperature_2m").get<double>();
    cc.feels_like   = current.at("apparent_temperature").get<double>();
    cc.humidity     = current.at("relative_humidity_2m").get<int>();
    cc.wind_speed   = current.at("wind_speed_10m").get<double>();
    cc.weather_code = current.at("weather_code").get<int>();
    cc.is_day       = current.at("is_day").get<int>();

    // daily arrays always have at least one entry when forecast_days=1
    cc.daily_max = daily.at("temperature_2m_max").at(0).get<double>();
    cc.daily_min = daily.at("temperature_2m_min").at(0).get<double>();

    return cc;
}

std::optional<CurrentConditions> WeatherService::FetchCurrentConditions(
    double latitude, double longitude) {
    try {
        const std::string url      = BuildUrl(latitude, longitude);
        const std::string response = HttpClient::Fetch(url);
        return ParseCurrentConditions(response);
    } catch (const std::exception&) {
        // Network error, non-200 HTTP status, or malformed JSON — return
        // nullopt so callers display a graceful fallback instead of crashing.
        return std::nullopt;
    }
}

}  // namespace weather_cli
