#include <catch2/catch_test_macros.hpp>
#include "util/constants.hpp"
#include <string_view>

TEST_CASE("kCountryList invariants", "[util][constants]") {
    using namespace weather_cli;

    SECTION("kCountryList is non-empty") {
        REQUIRE(kCountryList.size() > 0);
    }

    SECTION("Index 0 is Australia (AU) — the default filter") {
        REQUIRE(kCountryList[0].code  == "AU");
        REQUIRE(kCountryList[0].label == "Australia");
    }

    SECTION("Every entry has a non-empty label") {
        for (const auto& entry : kCountryList) {
            REQUIRE_FALSE(entry.label.empty());
        }
    }

    SECTION("Exactly one entry has an empty code (Any Country sentinel)") {
        int empty_code_count = 0;
        for (const auto& entry : kCountryList) {
            if (entry.code.empty()) ++empty_code_count;
        }
        REQUIRE(empty_code_count == 1);
    }

    SECTION("The Any Country sentinel has label 'Any Country'") {
        bool found = false;
        for (const auto& entry : kCountryList) {
            if (entry.code.empty()) {
                REQUIRE(entry.label == "Any Country");
                found = true;
            }
        }
        REQUIRE(found);
    }

    SECTION("All non-sentinel codes are exactly 2 uppercase characters") {
        for (const auto& entry : kCountryList) {
            if (entry.code.empty()) continue;  // skip the sentinel
            REQUIRE(entry.code.size() == 2);
            for (char c : entry.code) {
                REQUIRE((c >= 'A' && c <= 'Z'));
            }
        }
    }

    SECTION("No duplicate country codes") {
        for (std::size_t i = 0; i < kCountryList.size(); ++i) {
            for (std::size_t j = i + 1; j < kCountryList.size(); ++j) {
                // Two empty codes would break SetCountryFilter logic.
                if (kCountryList[i].code.empty() && kCountryList[j].code.empty()) {
                    FAIL("Duplicate empty codes at indices " << i << " and " << j);
                }
                if (!kCountryList[i].code.empty() &&
                    kCountryList[i].code == kCountryList[j].code) {
                    FAIL("Duplicate code '" << kCountryList[i].code
                         << "' at indices " << i << " and " << j);
                }
            }
        }
    }
}
