#pragma once

#include "model/location_search_state.hpp"
#include <string>
#include <vector>

namespace weather_cli {

class GeocodingService {
public:
    // Sends query to geocoding API and returns matching locations list.
    static std::vector<LocationMatch> Search(const std::string& query);

    // Parses raw JSON response strings from the Geocoding API (for testing)
    static std::vector<LocationMatch> ParseResponse(const std::string& json_str);
};


}  // namespace weather_cli
