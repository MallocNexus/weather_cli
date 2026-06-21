#pragma once

#include "model/app_state.hpp"
#include <string>
#include <vector>

namespace weather_cli {

class GeocodingService {
public:
    // Sends query to geocoding API and returns matching locations list.
    static std::vector<LocationMatch> Search(const std::string& query);
};

}  // namespace weather_cli
