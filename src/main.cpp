#include <iostream>
#include <filesystem>

#include "../include/database.hpp"
#include "../include/api_server.hpp"

int main() {
    std::cout << "Starting Smart Hub..." << std::endl;

    const std::filesystem::path databasePath{"storage/Personal.db"};
    std::error_code filesystemError;
    std::filesystem::create_directories(databasePath.parent_path(), filesystemError);
    if (filesystemError) {
      std::cerr << "Failed to create database directory: "
                << filesystemError.message() << '\n';
      return 1;
    }

    personalAPI::Database db(databasePath.string());

    if (!db.open()) {
      std::cerr << "Failed to initialize database connection: "
                << db.getLastError() << '\n';
      return 1;
    }

    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS metrics ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "source TEXT NOT NULL, " // garmin rowing kobo
        "payload TEXT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);"
        )) {
      std::cerr << "Failed to create metrics table: "
                << db.getLastError() << '\n';
      return 1;
    }

    // list all tables managed by your api backend
    auto tables = db.listTables();
    std::cout << "active tables in local database: \n";
    for (const auto& table : tables) {
      std::cout << " - " << table << "\n";
    }
    return 0;
}
