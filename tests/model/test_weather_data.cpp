#include <catch2/catch_test_macros.hpp>
#include "model/weather_data.hpp"

TEST_CASE("CurrentConditions default initialisation", "[model][weather_data]") {
    using namespace weather_cli;

    SECTION("All numeric fields default to zero") {
        CurrentConditions cc;
        REQUIRE(cc.temperature  == 0.0);
        REQUIRE(cc.feels_like   == 0.0);
        REQUIRE(cc.humidity     == 0);
        REQUIRE(cc.wind_speed   == 0.0);
        REQUIRE(cc.weather_code == 0);
        REQUIRE(cc.daily_max    == 0.0);
        REQUIRE(cc.daily_min    == 0.0);
    }

    SECTION("Fields can be written and read back") {
        CurrentConditions cc;
        cc.temperature  = 22.3;
        cc.feels_like   = 20.1;
        cc.humidity     = 78;
        cc.wind_speed   = 14.5;
        cc.weather_code = 61;
        cc.daily_max    = 25.0;
        cc.daily_min    = 15.0;

        REQUIRE(cc.temperature  == 22.3);
        REQUIRE(cc.feels_like   == 20.1);
        REQUIRE(cc.humidity     == 78);
        REQUIRE(cc.wind_speed   == 14.5);
        REQUIRE(cc.weather_code == 61);
        REQUIRE(cc.daily_max    == 25.0);
        REQUIRE(cc.daily_min    == 15.0);
    }
}
