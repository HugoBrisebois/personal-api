cmake_minimum_required(VERSION 3.21)
project(smart_hub CXX)

# set C++ standard ro C++20 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flag defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ADD THIS LINE:
include(FetchContent)

# Dependencies (Fetched or Found)

# nlohmann/json (header-only json parser)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# cpp-httplib (the lightweight http server/client for local API endpoints)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

# Slint GUI Framework (ui engine)
FetchContent_Declare(
    slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
)
FetchContent_MakeAvailable(slint)

# find the system sqlite3
find_package(SQLite3 REQUIRED)

add_library(personal_db
  src/Database.cpp
)

target_include_directories(personal_db PUBLIC
  include
)

target_link_libraries(personal_db PRIVATE
  SQLite3::SQLite3
)

# Find standard threading package
find_package(Threads REQUIRED)

# Executable target Configuration

add_executable(smart_hub
    src/main.cpp
    src/database.cpp
    src/api_server.cpp
)

# process the declarative Slint UI markup file
slint_target_sources(smart_hub ui/appwindow.slint)

# local include directories
target_include_directories(smart_hub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Library linking
target_link_libraries(smart_hub PRIVATE
    Slint::Slint
    nlohmann_json::nlohmann_json
    httplib::httplib
    SQLite3::SQLite3
    Threads::Threads
    dl
)
