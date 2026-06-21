#pragma once

#include "model/location_search_state.hpp"
#include <string>
#include <vector>

namespace weather_cli {

class ZippopotamService {
public:
    // Sends query to Zippopotam API and returns matching locations list.
    static std::vector<LocationMatch> Search(const std::string& postcode,
                                             const std::string& country_code);

    // Parses raw JSON response strings from Zippopotam.us (for testing)
    static std::vector<LocationMatch> ParseResponse(const std::string& json_str);
};

}  // namespace weather_cli
