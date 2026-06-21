#include "service/zippopotam_service.hpp"
#include "service/http_client.hpp"
#include <nlohmann/json.hpp>
#include <cpr/util.h>

#include <algorithm>
#include <cctype>

namespace {
std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return lower_str;
}
} // namespace

namespace weather_cli {

using json = nlohmann::json;

std::vector<LocationMatch> ZippopotamService::Search(const std::string& postcode,
                                                     const std::string& country_code) {
    if (postcode.empty() || country_code.empty()) {
        return {};
    }

    std::string postcode_encoded = cpr::util::urlEncode(postcode);
    std::string country_lower = to_lower(country_code);
    
    std::string url = "https://api.zippopotam.us/" + country_lower + "/" + postcode_encoded;
    std::string response = HttpClient::Fetch(url);
    if (response.empty() || response == "{}") {
        return {};
    }
    
    return ParseResponse(response);
}

std::vector<LocationMatch> ZippopotamService::ParseResponse(const std::string& json_str) {
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
