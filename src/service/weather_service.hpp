#pragma once

#include "model/weather_data.hpp"
#include <optional>
#include <string>

namespace weather_cli {

// WeatherService fetches and parses current weather conditions from the
// Open-Meteo /v1/forecast endpoint.
class WeatherService {
public:
    // Fetches current conditions for the given latitude/longitude.
    // Returns std::nullopt on any network error or JSON parse failure so
    // callers never need to handle exceptions.
    static std::optional<CurrentConditions> FetchCurrentConditions(
        double latitude, double longitude);

    // Parses a raw Open-Meteo JSON response string into CurrentConditions.
    // Exposed as public so unit tests can call it with mock JSON without
    // making real network requests.
    // Throws nlohmann::json::exception on malformed input.
    static CurrentConditions ParseCurrentConditions(const std::string& json_str);
};

}  // namespace weather_cli
