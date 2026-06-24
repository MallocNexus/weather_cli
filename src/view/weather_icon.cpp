#include "view/weather_icon.hpp"
#include "view/icons/weather_icons.hpp"

namespace weather_cli {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Returns the icon constant for the given WMO code.
static const std::vector<std::string>& IconForCode(int wmo_code, int is_day) {
    // WMO 0: Clear sky
    if (wmo_code == 0) return is_day ? icons::kSunny : icons::kClearNight;

    // WMO 1–2: Mainly clear / partly cloudy
    if (wmo_code == 1 || wmo_code == 2) return is_day ? icons::kSunny : icons::kClearNight;

    // WMO 3: Overcast
    if (wmo_code == 3) return icons::kCloudy;

    // WMO 45, 48: Fog / depositing rime fog
    if (wmo_code == 45 || wmo_code == 48) return icons::kCloudy;

    // WMO 51–57: Drizzle (light, moderate, dense; freezing variants)
    if (wmo_code >= 51 && wmo_code <= 57) return icons::kRainy;

    // WMO 61–67: Rain (slight, moderate, heavy; freezing variants)
    if (wmo_code >= 61 && wmo_code <= 67) return icons::kRainy;

    // WMO 71–77: Snowfall / snow grains
    if (wmo_code >= 71 && wmo_code <= 77) return icons::kSnowy;

    // WMO 80–82: Rain showers (slight, moderate, violent)
    if (wmo_code >= 80 && wmo_code <= 82) return icons::kRainy;

    // WMO 85–86: Snow showers (slight, heavy)
    if (wmo_code == 85 || wmo_code == 86) return icons::kSnowy;

    // WMO 95: Thunderstorm
    if (wmo_code == 95) return icons::kStormy;

    // WMO 96, 99: Thunderstorm with slight/heavy hail
    if (wmo_code == 96 || wmo_code == 99) return icons::kStormy;

    // Fallback for any unrecognised code
    return icons::kCloudy;
}

// Returns the condition description string for the given WMO code.
static std::string DescriptionForCode(int wmo_code) {
    switch (wmo_code) {
        case 0:  return "Clear Sky";
        case 1:  return "Mainly Clear";
        case 2:  return "Partly Cloudy";
        case 3:  return "Overcast";
        case 45: return "Foggy";
        case 48: return "Rime Fog";
        case 51: return "Light Drizzle";
        case 53: return "Moderate Drizzle";
        case 55: return "Dense Drizzle";
        case 56: return "Light Freezing Drizzle";
        case 57: return "Heavy Freezing Drizzle";
        case 61: return "Light Rain";
        case 63: return "Moderate Rain";
        case 65: return "Heavy Rain";
        case 66: return "Light Freezing Rain";
        case 67: return "Heavy Freezing Rain";
        case 71: return "Light Snow";
        case 73: return "Moderate Snow";
        case 75: return "Heavy Snow";
        case 77: return "Snow Grains";
        case 80: return "Light Rain Showers";
        case 81: return "Moderate Rain Showers";
        case 82: return "Violent Rain Showers";
        case 85: return "Light Snow Showers";
        case 86: return "Heavy Snow Showers";
        case 95: return "Thunderstorm";
        case 96: return "Thunderstorm w/ Hail";
        case 99: return "Thunderstorm w/ Heavy Hail";
        default: return "Unknown";
    }
}

// ---------------------------------------------------------------------------
// WeatherIcon public API
// ---------------------------------------------------------------------------

const std::vector<std::string>& WeatherIcon::GetIcon(int wmo_code, int is_day) {
    return IconForCode(wmo_code, is_day);
}

std::string WeatherIcon::GetDescription(int wmo_code) {
    return DescriptionForCode(wmo_code);
}

}  // namespace weather_cli
