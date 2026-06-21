#pragma once

#include "model/location_repository.hpp"
#include <functional>
#include <string>

namespace weather_cli {

class DatabaseController {
public:
    DatabaseController(LocationRepository& repo, std::function<void()> trigger_redraw);

    // Initialize database tables if not existing
    bool InitializeDatabase();

    // Save a new location to the database (Favorites)
    bool SaveLocation(const LocationMatch& location);

    // Retrieve all saved locations and populate the repository
    bool LoadSavedLocations();

    LocationRepository& GetRepository() { return repo_; }
    const LocationRepository& GetRepository() const { return repo_; }

private:
    LocationRepository& repo_;
    std::function<void()> trigger_redraw_;
};

}  // namespace weather_cli
