#include <catch2/catch_test_macros.hpp>
#include "view/weather_icon.hpp"
#include "view/icons/weather_icons.hpp"

TEST_CASE("WeatherIcon::GetIcon", "[view][weather_icon]") {
    using namespace weather_cli;

    SECTION("WMO 0 — Clear sky maps to kSunny") {
        REQUIRE(WeatherIcon::GetIcon(0) == icons::kSunny);
    }

    SECTION("WMO 1 — Mainly clear maps to kSunny") {
        REQUIRE(WeatherIcon::GetIcon(1) == icons::kSunny);
    }

    SECTION("WMO 2 — Partly cloudy maps to kSunny") {
        REQUIRE(WeatherIcon::GetIcon(2) == icons::kSunny);
    }

    SECTION("WMO 3 — Overcast maps to kCloudy") {
        REQUIRE(WeatherIcon::GetIcon(3) == icons::kCloudy);
    }

    SECTION("WMO 45 — Fog maps to kCloudy") {
        REQUIRE(WeatherIcon::GetIcon(45) == icons::kCloudy);
    }

    SECTION("WMO 48 — Rime fog maps to kCloudy") {
        REQUIRE(WeatherIcon::GetIcon(48) == icons::kCloudy);
    }

    SECTION("WMO 51 — Light drizzle maps to kRainy") {
        REQUIRE(WeatherIcon::GetIcon(51) == icons::kRainy);
    }

    SECTION("WMO 61 — Light rain maps to kRainy") {
        REQUIRE(WeatherIcon::GetIcon(61) == icons::kRainy);
    }

    SECTION("WMO 65 — Heavy rain maps to kRainy") {
        REQUIRE(WeatherIcon::GetIcon(65) == icons::kRainy);
    }

    SECTION("WMO 71 — Light snow maps to kSnowy") {
        REQUIRE(WeatherIcon::GetIcon(71) == icons::kSnowy);
    }

    SECTION("WMO 77 — Snow grains maps to kSnowy") {
        REQUIRE(WeatherIcon::GetIcon(77) == icons::kSnowy);
    }

    SECTION("WMO 80 — Light rain showers maps to kRainy") {
        REQUIRE(WeatherIcon::GetIcon(80) == icons::kRainy);
    }

    SECTION("WMO 85 — Light snow showers maps to kSnowy") {
        REQUIRE(WeatherIcon::GetIcon(85) == icons::kSnowy);
    }

    SECTION("WMO 95 — Thunderstorm maps to kStormy") {
        REQUIRE(WeatherIcon::GetIcon(95) == icons::kStormy);
    }

    SECTION("WMO 96 — Thunderstorm w/ hail maps to kStormy") {
        REQUIRE(WeatherIcon::GetIcon(96) == icons::kStormy);
    }

    SECTION("WMO 99 — Thunderstorm w/ heavy hail maps to kStormy") {
        REQUIRE(WeatherIcon::GetIcon(99) == icons::kStormy);
    }

    SECTION("Unknown WMO code falls back to kCloudy") {
        REQUIRE(WeatherIcon::GetIcon(999) == icons::kCloudy);
        REQUIRE(WeatherIcon::GetIcon(-1)  == icons::kCloudy);
    }

    SECTION("Returned icon vectors have at least 4 lines") {
        // kSunny has 5 lines; all others have 4. Verify no lookup returns
        // a partial or empty icon.
        for (int code : {0, 1, 2, 3, 45, 48, 51, 61, 71, 80, 85, 95, 96, 99}) {
            REQUIRE(WeatherIcon::GetIcon(code).size() >= 4);
        }
    }
}

TEST_CASE("WeatherIcon::GetDescription", "[view][weather_icon]") {
    using namespace weather_cli;

    SECTION("Clear sky") {
        REQUIRE(WeatherIcon::GetDescription(0) == "Clear Sky");
    }

    SECTION("Mainly clear") {
        REQUIRE(WeatherIcon::GetDescription(1) == "Mainly Clear");
    }

    SECTION("Partly cloudy") {
        REQUIRE(WeatherIcon::GetDescription(2) == "Partly Cloudy");
    }

    SECTION("Overcast") {
        REQUIRE(WeatherIcon::GetDescription(3) == "Overcast");
    }

    SECTION("Foggy") {
        REQUIRE(WeatherIcon::GetDescription(45) == "Foggy");
    }

    SECTION("Light drizzle") {
        REQUIRE(WeatherIcon::GetDescription(51) == "Light Drizzle");
    }

    SECTION("Moderate drizzle") {
        REQUIRE(WeatherIcon::GetDescription(53) == "Moderate Drizzle");
    }

    SECTION("Light rain") {
        REQUIRE(WeatherIcon::GetDescription(61) == "Light Rain");
    }

    SECTION("Moderate rain") {
        REQUIRE(WeatherIcon::GetDescription(63) == "Moderate Rain");
    }

    SECTION("Heavy rain") {
        REQUIRE(WeatherIcon::GetDescription(65) == "Heavy Rain");
    }

    SECTION("Light snow") {
        REQUIRE(WeatherIcon::GetDescription(71) == "Light Snow");
    }

    SECTION("Heavy snow") {
        REQUIRE(WeatherIcon::GetDescription(75) == "Heavy Snow");
    }

    SECTION("Light rain showers") {
        REQUIRE(WeatherIcon::GetDescription(80) == "Light Rain Showers");
    }

    SECTION("Thunderstorm") {
        REQUIRE(WeatherIcon::GetDescription(95) == "Thunderstorm");
    }

    SECTION("Thunderstorm with hail") {
        REQUIRE(WeatherIcon::GetDescription(96) == "Thunderstorm w/ Hail");
    }

    SECTION("Thunderstorm with heavy hail") {
        REQUIRE(WeatherIcon::GetDescription(99) == "Thunderstorm w/ Heavy Hail");
    }

    SECTION("Unknown code returns 'Unknown'") {
        REQUIRE(WeatherIcon::GetDescription(999) == "Unknown");
        REQUIRE(WeatherIcon::GetDescription(-1)  == "Unknown");
    }
}
