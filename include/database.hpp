#pragma once // ensures this file is only processed once per compilation

#include <string>
#include <sqlite3.h>

class Database {
    private:
        sqlite3* db;

    public:
        // constructor and destructor declarations
        Database(const std::string& db_path);
        ~Database();
        
        // member function declaration
        void execute_query(const std::string& query);
}