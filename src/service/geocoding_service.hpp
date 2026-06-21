#pragma once

#include "model/location_search_state.hpp"
#include <string>
#include <vector>

namespace weather_cli {

class GeocodingService {
public:
    // Sends query to geocoding API and returns matching locations list.
    // country_code is an optional ISO 3166-1 alpha-2 filter (e.g. "AU", "US").
    static std::vector<LocationMatch> Search(const std::string& query,
                                             const std::string& country_code = "");

    // Parses raw JSON response strings from the Geocoding API (for testing)
    static std::vector<LocationMatch> ParseResponse(const std::string& json_str);
};


}  // namespace weather_cli
