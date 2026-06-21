#include <catch2/catch_test_macros.hpp>
#include "service/zippopotam_service.hpp"
#include <string>

TEST_CASE("Zippopotam Response Parsing", "[service][zippopotam]") {
    using namespace weather_cli;

    SECTION("Parses valid Zippopotam JSON response correctly") {
        std::string json_str = R"({
            "post code": "2154",
            "country": "Australia",
            "country abbreviation": "AU",
            "places": [
                {
                    "place name": "Castle Hill",
                    "longitude": "151",
                    "state": "New South Wales",
                    "state abbreviation": "NSW",
                    "latitude": "-33.7333"
                }
            ]
        })";

        auto results = ZippopotamService::ParseResponse(json_str);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].name == "Castle Hill");
        REQUIRE(results[0].country == "Australia");
        REQUIRE(results[0].region == "New South Wales");
        REQUIRE(results[0].latitude == -33.7333);
        REQUIRE(results[0].longitude == 151.0);
    }
}
