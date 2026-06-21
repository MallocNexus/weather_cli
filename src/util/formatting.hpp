#pragma once

#include <string>

namespace weather_cli {

double CelsiusToFahrenheit(double celsius);
std::string FormatTemperature(double temp, bool is_celsius);
std::string MapWmoCodeToDescription(int code);

}  // namespace weather_cli
