#pragma once

#include "model/location_search_state.hpp"
#include <vector>
#include <string>
#include <sqlite3.h>

namespace weather_cli {

class LocationRepository {
public:
    explicit LocationRepository(const std::string& db_path);
    ~LocationRepository();

    // Prevent copying
    LocationRepository(const LocationRepository&) = delete;
    LocationRepository& operator=(const LocationRepository&) = delete;

    bool Initialize();
    bool SaveLocation(const LocationMatch& location);
    bool LoadSavedLocations();

    const std::vector<LocationMatch>& GetSavedLocations() const { return saved_locations_; }

private:
    bool ExecuteQuery(const std::string& sql);

    std::string db_path_;
    sqlite3* db_ = nullptr;
    std::vector<LocationMatch> saved_locations_;
};

}  // namespace weather_cli
