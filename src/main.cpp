#include <iostream>
#include <thread>

// include your custom project headers
#include "database.hpp"
#include "api_server.hpp"

int main() {
    std::cout << "Starting Smart Hub..." << std::endl;

    // init the sqlite database
    PersonalAPI::Database db("storage/Personal.db");

    if (!db.isOpen()) {
      std:cerr << "Failed to initilize database connection. \n";
      return 1;
    }

    // set up a metrics table for your data aggregator
    db.execute(
        "CREATE TABLE IF NOT EXISTS metrics ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "source TEXT NOT NULL, " // garmin rowing kobo
        "payload TEXT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);"
        );

    // list all tables managed by your api backend
    auto tables = db.listTables();
    std::cout << "active tables in local database: \n";
    for (const auto& table : tables) {
      std::cout << " - " << table << "\n";
    }
    return 0;
}
