#include "service/geocoding_service.hpp"
#include "service/http_client.hpp"
#include <nlohmann/json.hpp>
#include <cpr/util.h>
#include <iostream>

namespace weather_cli {

using json = nlohmann::json;

std::vector<LocationMatch> GeocodingService::Search(const std::string& query) {
    if (query.empty()) {
        return {};
    }

    std::vector<LocationMatch> matches;
    try {
        // Safe query encoding using cpr's urlEncode helper
        std::string query_encoded = cpr::util::urlEncode(query);
        std::string url = "https://geocoding-api.open-meteo.com/v1/search?name=" + 
                          query_encoded + "&count=5&language=en&format=json";

        std::string response = HttpClient::Fetch(url);
        auto data = json::parse(response);

        if (data.contains("results") && data["results"].is_array()) {
            for (const auto& item : data["results"]) {
                LocationMatch match;
                match.name = item.value("name", "");
                match.country = item.value("country", "");
                match.region = item.value("admin1", ""); // admin1 represents state/region
                match.latitude = item.value("latitude", 0.0);
                match.longitude = item.value("longitude", 0.0);
                matches.push_back(match);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Geocoding request error: " << e.what() << "\n";
    }
    return matches;
}

}  // namespace weather_cli
