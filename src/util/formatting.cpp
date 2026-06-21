#include "util/formatting.hpp"
#include <sstream>
#include <iomanip>

namespace weather_cli {

double CelsiusToFahrenheit(double celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

std::string FormatTemperature(double temp, bool is_celsius) {
    double value = is_celsius ? temp : CelsiusToFahrenheit(temp);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << (is_celsius ? "°C" : "°F");
    return ss.str();
}

std::string MapWmoCodeToDescription(int code) {
    switch (code) {
        case 0: return "Clear sky";
        case 1:
        case 2:
        case 3: return "Partly cloudy";
        case 51:
        case 53:
        case 55: return "Drizzle";
        case 61:
        case 63:
        case 65: return "Rain";
        case 71:
        case 73:
        case 75: return "Snow fall";
        case 80:
        case 81:
        case 82: return "Rain showers";
        case 95: return "Thunderstorm";
        default: return "Unknown Condition";
    }
}

}  // namespace weather_cli
