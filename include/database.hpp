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

        // initialization function declaration
        void createDB(const std::string& db_path);

        // execution function declaration
        void execute_query(const std::string& query);

        // table creatation function declaration
        void createTable(const std::string& query);

        // list a tables function declaration
        void listTable(const string& query);
        
        // list content of a table function
        void listContent(const string& query);
}
