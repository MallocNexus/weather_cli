#include "model/location_repository.hpp"
#include "util/constants.hpp"
#include <sqlite3.h>
#include <iostream>
#include <format>

namespace weather_cli {

LocationRepository::LocationRepository(const std::string& db_path)
    : db_path_(db_path) {}

LocationRepository::~LocationRepository() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool LocationRepository::Initialize() {
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        if (db_) {
            std::cerr << "Error: Can't open database: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_close(db_);
            db_ = nullptr;
        }
        return false;
    }

    std::string sql = std::format(
        "CREATE TABLE IF NOT EXISTS {} ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "{} TEXT NOT NULL,"
        "{} TEXT,"
        "{} TEXT,"
        "{} REAL NOT NULL,"
        "{} REAL NOT NULL"
        ");",
        kDbLocationTable,
        kDbLocationColName,
        kDbLocationColCountry,
        kDbLocationColRegion,
        kDbLocationColLatitude,
        kDbLocationColLongitude
    );

    return ExecuteQuery(sql);
}

bool LocationRepository::SaveLocation(const LocationMatch& location) {
    if (!db_) return false;

    // Check if location already exists to prevent duplicates
    std::string select_sql = std::format(
        "SELECT id FROM {} WHERE {} = ? AND {} = ? AND {} = ? LIMIT 1;",
        kDbLocationTable,
        kDbLocationColName,
        kDbLocationColLatitude,
        kDbLocationColLongitude
    );

    sqlite3_stmt* select_stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, select_sql.c_str(), -1, &select_stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(select_stmt, 1, location.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(select_stmt, 2, location.latitude);
        sqlite3_bind_double(select_stmt, 3, location.longitude);
        if (sqlite3_step(select_stmt) == SQLITE_ROW) {
            // Already exists, just close and return success
            sqlite3_finalize(select_stmt);
            return true;
        }
        sqlite3_finalize(select_stmt);
    }

    std::string insert_sql = std::format(
        "INSERT INTO {} ({}, {}, {}, {}, {}) VALUES (?, ?, ?, ?, ?);",
        kDbLocationTable,
        kDbLocationColName,
        kDbLocationColCountry,
        kDbLocationColRegion,
        kDbLocationColLatitude,
        kDbLocationColLongitude
    );

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db_, insert_sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error: Failed to prepare insert query: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, location.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, location.country.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, location.region.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, location.latitude);
    sqlite3_bind_double(stmt, 5, location.longitude);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Error: Failed to insert location record: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Reload locations into repo
    return LoadSavedLocations();
}

bool LocationRepository::LoadSavedLocations() {
    if (!db_) return false;

    std::string sql = std::format(
        "SELECT {}, {}, {}, {}, {} FROM {} ORDER BY id ASC;",
        kDbLocationColName,
        kDbLocationColCountry,
        kDbLocationColRegion,
        kDbLocationColLatitude,
        kDbLocationColLongitude,
        kDbLocationTable
    );

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error: Failed to prepare select query: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    std::vector<LocationMatch> locations;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LocationMatch loc;
        loc.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        
        const unsigned char* country = sqlite3_column_text(stmt, 1);
        if (country) loc.country = reinterpret_cast<const char*>(country);
        
        const unsigned char* region = sqlite3_column_text(stmt, 2);
        if (region) loc.region = reinterpret_cast<const char*>(region);

        loc.latitude = sqlite3_column_double(stmt, 3);
        loc.longitude = sqlite3_column_double(stmt, 4);

        locations.push_back(loc);
    }

    sqlite3_finalize(stmt);
    saved_locations_ = std::move(locations);
    return true;
}

bool LocationRepository::ExecuteQuery(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

}  // namespace weather_cli
