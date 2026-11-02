cmake_minimum_required(VERSION 3.21)
project(smart_hub CXX)

# set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimization flag defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# Dependencies (Fetched or Found)

# pybind11 (embedded Python support)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

set(PYBIND11_FINDPYTHON ON)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v3.0.1
)
FetchContent_MakeAvailable(pybind11)

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

# Find standard threading package
find_package(Threads REQUIRED)

if(UNIX AND NOT APPLE)
    find_package(Fontconfig REQUIRED)
endif()

# Executable target Configuration
add_executable(smart_hub
    src/main.cpp
    src/database.cpp
    src/api_server.cpp
)

# Process the declarative Slint UI markup file
slint_target_sources(smart_hub ui/appWindow.slint)

# Local include directories (including CMAKE_CURRENT_BINARY_DIR for generated Slint headers)
target_include_directories(smart_hub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Library linking
target_link_libraries(smart_hub PRIVATE
    Slint::Slint
    pybind11::embed
    nlohmann_json::nlohmann_json
    httplib::httplib
    SQLite3::SQLite3
    Threads::Threads
    dl
)

if(UNIX AND NOT APPLE)
    target_link_libraries(smart_hub PRIVATE Fontconfig::Fontconfig)
endif()