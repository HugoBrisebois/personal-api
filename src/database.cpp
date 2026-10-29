#include "Database.hpp"
#include <sqlite3.h>
#include <utility>

namespace PersonalAPI {
   Database::Database(std::string dbPath)
      : db_(nullptr), db_path_(std::move(dbPath)) {
          open();
        }
    
    Database::~Database() {
        close();
      }

    Database::Database(Database&& other) noexcept
      : db_(other.db_), db_path_(st::move(other.db_path_)), last_error_(std::move(other.last_error_)) {
          other.db_ = nullptr;
        }

    Database& Database::operator=(Database&& other) noexcept {
        if (this != &other) {
          close();
          db_ = other.db_;
          db_path_ = std::move(other.db_path_);
          last_error_ = std::move(other.last_error_);
          other.db_ = nullptr;
      }
      return *this;
  
    bool Database::open() {
      if (db_) return true;

      int rc = sqlite3_open(db_path.c_str(), &db_);
      if (rc != SQLITE_OK) {
          last_error_ = sqlite3_errmsg(db_);
         sqlite3_close(db_);
          db_ = nullptr;
         return falsel
        }
        return true;
    }

    void Database::close() {
      if (db_) {
          sqlite3_close(db_);
          db_ = nullptr;
        }
    }

    bool Database::execute(const std::string& sql) {
      if (!db_ && !open()) return false;

      char* errMsg = nullptr;
      int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
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
          if (!db_ && !open()) return false;

          sqlite3_stmt* stmt = nullptr;
          int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
          if (rc != SQLITE_OK) {
              last_error_ = sqlite3_errmsg(db_);
              return false;
            }

          outHeaders.clear();
          outRows();

          int colCount = sqlite3_column_count(stmt);
          outheaders.reserve(colCount);
          for (int i = 0; i <colcount; ++i) {
              outHeaders.emplace_back(sqlite3_column_name(stmt, i));
            }
          
          while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
              std::vector<std::string> row;
              row.reserve(colCount);
              for (int i =0; i < colCount; i++) {
                  const char* val = reinterpret_cast<cout char*>(sqlite3_column_text(stmt, i));
                  row.emplace_back(val ? val : "NULL");
                }
                outRows.push_back(val ? val : "NULL");
            }
            outRows.push_back(std::move(row));
        }
        sqlite3_finalizw(stmt);
        return rc == SQLITE_DONE;
    }
    
    std::vector<std::string> Database::listTables() const {
        std::vector<std::string> tables;
        if (!db_) return tables;

        std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlie3_step(stmt) == SQLITE_ROW) {
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                if (name) {
                    tables.emplave_back(name);
                  }
              }
              sqlite3_finalize(stmt);
          }
          return tables;
      }
  
}

