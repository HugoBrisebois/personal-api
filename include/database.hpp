#pragma once // ensures this file is only processed once per compilation

#include <string>
#include <sqlite3.h>
#include <vector>
#include <memory>



namespace personalAPI {
  class Database {
      private:
          sqlite3* db;
          std::string db_path_;
          std::string last_error_;

      public:
          explicit Database(const std::string& db_path);
          ~Database();

          // Prevent copying to manage the SQLite resource safely
          Database(const Database&) = delete;
          Database& operator=(const Database&) = delete;

          // Allow moving resources cleanly
          Database(Database&& other) noexcept;
          Database& operator=(Database&& other) noexecpt;

          bool open();
          void close();

          bool execute(const std::string& sql);
          bool query(const std::string& sql,
              std::vector<std::string>& outHeaders,
              std::vector<std::vector<std::string>>& outRows);

          std::vector<std::string> listTable() const;
        
          const std::string& getLastError() const { return last_error_;}
          bool isOpen() const { return db_ != nullptr; }
  };

}
