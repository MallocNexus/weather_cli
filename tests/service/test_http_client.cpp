#include <catch2/catch_test_macros.hpp>
#include "service/http_client.hpp"
#include <stdexcept>

TEST_CASE("HttpClient Error Handling", "[service][http]") {
    using namespace weather_cli;

    SECTION("Malformed protocol throws exception") {
        // CPR/curl will fail on invalid protocols
        REQUIRE_THROWS_AS(HttpClient::Fetch("invalid_protocol://localhost"), std::runtime_error);
    }

    SECTION("Empty URL throws exception") {
        REQUIRE_THROWS_AS(HttpClient::Fetch(""), std::runtime_error);
    }
}
