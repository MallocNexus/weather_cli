#pragma once

#include <string>
#include <vector>

namespace weather_cli {

// WeatherIcon maps WMO weather interpretation codes to ASCII art icons and
// human-readable condition descriptions. Icons are sourced from
// view/icons/weather_icons.hpp.
//
// WMO code reference: https://open-meteo.com/en/docs#weathervariables
class WeatherIcon {
public:
    // Returns the 4-line ASCII art vector for the given WMO code.
    // Always returns a valid reference — unknown codes fall back to kCloudy.
    static const std::vector<std::string>& GetIcon(int wmo_code);

    // Returns a human-readable condition description for the given WMO code.
    // Unknown codes return "Unknown".
    static std::string GetDescription(int wmo_code);
};

}  // namespace weather_cli
