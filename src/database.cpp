#include "../include/database.hpp"
#include <sqlite3.h>
#include <utility>

namespace personalAPI {
   Database::Database(const std::string& dbPath)
      : db(nullptr), db_path(std::move(dbPath)) {
        }
    
    Database::~Database() {
        close();
      }

    Database::Database(Database&& other) noexcept
      : db(other.db),
        db_path(std::move(other.db_path)),
        last_error(std::move(other.last_error)) {
          other.db = nullptr;
        }
      }

    Database& Database::operator=(Database&& other) noexcept {
        if (this != &other) {
          close();
          if (db) sqlite3_close(db);
          db = other.db;
          db_path = std::move(other.db_path);
          last_error = std::move(other.last_error);
          other.db = nullptr;
      }
      return *this;
    }

    bool Database::open() {
      if (db_) return true;

      int rc = sqlite3_open(db_path.c_str(), &db);
      if (rc != SQLITE_OK) {
          last_error_ = sqlite3_errmsg(db);
         sqlite3_close(db);
          db_ = nullptr;
         return false;
        }
        return true;
    }

    void Database::close() {
      if (db) {
          sqlite3_close(db);
          db_ = nullptr;
        }
    }

    bool Database::execute(const std::string& sql) {
      if (!db && !open()) return false;

      char* errMsg = nullptr;
      int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
      if(rc != SQLITE_OK) {
          last_error_ = errMsg ? errMsg : "Unknown execution error";
          sqlite3_free(errMsg);
          return false;
      }
       return true;
    }

    bool Database::query(const std::string& sql,
                     std::vector<std::string>& outHeaders,
                     std::vector<std::vector<std::string>>& outRows) {
    if (!db && !open()) return false;

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        last_error = sqlite3_errmsg(db);
        return false;
    }

    int colCount = sqlite3_column_count(stmt);
    outHeaders.clear();
    outHeaders.reserve(colCount);
    for (int i = 0; i < colCount; ++i) {
        outHeaders.push_back(sqlite3_column_name(stmt, i));
    }

    outRows.clear();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        row.reserve(colCount);
        for (int i = 0; i < colCount; ++i) {
            const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(val ? val : "NULL");
        }
        outRows.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    return true;
}
    
    std::vector<std::string> Database::listTables() const {
        std::vector<std::string> tables;
        if (!db) return tables;

        std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                if (name) {
                    tables.emplace_back(name);
                }
              }
              sqlite3_finalize(stmt);
          }
          return tables;
    }


