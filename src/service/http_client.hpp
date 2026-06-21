#pragma once

#include <string>

namespace weather_cli {

class HttpClient {
public:
    // Sends a synchronous GET request and returns the response payload text.
    // Throws std::runtime_error on non-200 responses or connection failures.
    static std::string Fetch(const std::string& url);
};

}  // namespace weather_cli
