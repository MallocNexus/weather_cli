#include "service/geocoding_service.hpp"
#include "service/http_client.hpp"
#include <nlohmann/json.hpp>
#include <cpr/util.h>

#include <algorithm>
#include <cctype>

namespace {
bool is_numeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return lower_str;
}
} // namespace

namespace weather_cli {

using json = nlohmann::json;

std::vector<LocationMatch> GeocodingService::Search(const std::string& query,
                                                    const std::string& country_code) {
    if (query.empty()) {
        return {};
    }

    std::string query_encoded = cpr::util::urlEncode(query);

    if (is_numeric(query) && !country_code.empty()) {
        std::string country_lower = to_lower(country_code);
        std::string url = "https://api.zippopotam.us/" + country_lower + "/" + query_encoded;
        std::string response = HttpClient::Fetch(url);
        if (response.empty() || response == "{}") {
            return {};
        }
        return ParseZippopotamResponse(response);
    }

    std::string url = "https://geocoding-api.open-meteo.com/v1/search?name=" +
                      query_encoded + "&count=5&language=en&format=json";
    if (!country_code.empty()) {
        url += "&countryCode=" + cpr::util::urlEncode(country_code);
    }

    std::string response = HttpClient::Fetch(url);
    return ParseResponse(response);
}

std::vector<LocationMatch> GeocodingService::ParseResponse(const std::string& json_str) {
    if (json_str.empty() || json_str == "{}") {
        return {};
    }

    std::vector<LocationMatch> matches;
    try {
        auto data = json::parse(json_str);

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
        // Suppress parsing exceptions to return empty matches silently
    }
    return matches;
}

std::vector<LocationMatch> GeocodingService::ParseZippopotamResponse(const std::string& json_str) {
    if (json_str.empty() || json_str == "{}" || json_str.find("404") != std::string::npos) {
        return {};
    }

    std::vector<LocationMatch> matches;
    try {
        auto data = json::parse(json_str);

        if (data.contains("places") && data["places"].is_array() && !data["places"].empty()) {
            for (const auto& item : data["places"]) {
                LocationMatch match;
                match.name = item.value("place name", "");
                match.country = data.value("country", "");
                match.region = item.value("state", "");
                
                // Zippopotam returns lat/lon as strings, we need to convert them
                std::string lat_str = item.value("latitude", "0.0");
                std::string lon_str = item.value("longitude", "0.0");
                match.latitude = std::stod(lat_str);
                match.longitude = std::stod(lon_str);
                
                matches.push_back(match);
            }
        }
    } catch (const std::exception& e) {
        // Suppress parsing exceptions to return empty matches silently
    }
    return matches;
}

}  // namespace weather_cli
