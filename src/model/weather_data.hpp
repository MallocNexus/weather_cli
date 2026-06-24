#pragma once

namespace weather_cli {

// Represents a snapshot of current weather conditions returned by the
// Open-Meteo /v1/forecast endpoint (current= block + first day of daily= block).
struct CurrentConditions {
    // Temperature at 2 m above ground level, in degrees Celsius.
    double temperature = 0.0;

    // Apparent ("feels like") temperature at 2 m, in degrees Celsius.
    double feels_like = 0.0;

    // Relative humidity at 2 m, as an integer percentage (0–100).
    int humidity = 0;

    // Wind speed at 10 m above ground level, in km/h.
    double wind_speed = 0.0;

    // WMO Weather interpretation code.
    // See: https://open-meteo.com/en/docs#weathervariables
    int weather_code = 0;

    // Today's forecast maximum temperature, in degrees Celsius.
    double daily_max = 0.0;

    // Today's forecast minimum temperature, in degrees Celsius.
    double daily_min = 0.0;

    // 1 when the current time is daytime at the queried location, 0 at night.
    // Sourced from the Open-Meteo `is_day` current variable.
    int is_day = 1;
};

}  // namespace weather_cli
