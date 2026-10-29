#include <iostream>
#include <thread>

// include your custom project headers
#include "database.hpp"
#include "api_server.hpp"

int main() {
    std::cout << "Starting Smart Hub..." << std::endl;

    // init the sqlite database
    Database db("personal.db");
    db.init_db("personal.db");
    
}