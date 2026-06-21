#include "service/http_client.hpp"
#include <cpr/cpr.h>
#include <stdexcept>

namespace weather_cli {

std::string HttpClient::Fetch(const std::string& url) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    if (r.status_code != 200) {
        throw std::runtime_error("HTTP GET failed (status " + std::to_string(r.status_code) + "): " + r.error.message);
    }
    return r.text;
}

}  // namespace weather_cli
