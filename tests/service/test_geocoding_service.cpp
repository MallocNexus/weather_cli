#include <catch2/catch_test_macros.hpp>
#include "service/geocoding_service.hpp"
#include <string>

TEST_CASE("Geocoding Response Parsing", "[service][geocoding]") {
    using namespace weather_cli;

    SECTION("Parses valid JSON response correctly") {
        std::string json_str = R"({
            "results": [
                {
                    "name": "London",
                    "latitude": 51.50853,
                    "longitude": -0.12574,
                    "country": "United Kingdom",
                    "admin1": "England"
                },
                {
                    "name": "London",
                    "latitude": 42.98339,
                    "longitude": -81.23304,
                    "country": "Canada",
                    "admin1": "Ontario"
                }
            ]
        })";

        auto results = GeocodingService::ParseResponse(json_str);

        REQUIRE(results.size() == 2);

        REQUIRE(results[0].name == "London");
        REQUIRE(results[0].latitude == 51.50853);
        REQUIRE(results[0].longitude == -0.12574);
        REQUIRE(results[0].country == "United Kingdom");
        REQUIRE(results[0].region == "England");

        REQUIRE(results[1].name == "London");
        REQUIRE(results[1].latitude == 42.98339);
        REQUIRE(results[1].longitude == -81.23304);
        REQUIRE(results[1].country == "Canada");
        REQUIRE(results[1].region == "Ontario");
    }

    SECTION("Empty response returns empty list") {
        auto results = GeocodingService::ParseResponse("");
        REQUIRE(results.empty());
    }

    SECTION("Empty JSON object returns empty list") {
        auto results = GeocodingService::ParseResponse("{}");
        REQUIRE(results.empty());
    }

    SECTION("Malformed JSON returns empty list") {
        auto results = GeocodingService::ParseResponse("{invalid_json}");
        REQUIRE(results.empty());
    }

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

        auto results = GeocodingService::ParseZippopotamResponse(json_str);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].name == "Castle Hill");
        REQUIRE(results[0].country == "Australia");
        REQUIRE(results[0].region == "New South Wales");
        REQUIRE(results[0].latitude == -33.7333);
        REQUIRE(results[0].longitude == 151.0);
    }
}
