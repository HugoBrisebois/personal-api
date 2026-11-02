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
set(PYBIND11_FINDPYTHON ON)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Embed)

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
# Download the prebuilt Slint compiler instead of building the Rust compiler in
# this environment. The Rust-side compiler enables jemalloc by default for native
# builds, and that allocator fails during the downstream `tikv-jemalloc-sys`
# configure step here. Using the official release binary avoids the broken build path.
set(SLINT_COMPILER "download" CACHE STRING "Path to the slint-compiler executable" FORCE)
FetchContent_Declare(
    slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
)
FetchContent_MakeAvailable(slint)

if(NOT TARGET Slint::slint-compiler)
    set(_slint_release "v1.18.1")
    if(CMAKE_HOST_WIN32)
        set(_slint_platform "windows-${CMAKE_HOST_SYSTEM_PROCESSOR}")
        set(_slint_exe_suffix ".exe")
    else()
        set(_slint_platform "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
        set(_slint_exe_suffix "")
    endif()

    set(_slint_download_dir "${CMAKE_BINARY_DIR}/slint-prebuilt")
    set(_slint_archive "slint-compiler-${_slint_platform}.tar.gz")
    file(MAKE_DIRECTORY "${_slint_download_dir}")
    file(DOWNLOAD
        "https://github.com/slint-ui/slint/releases/download/${_slint_release}/${_slint_archive}"
        "${_slint_download_dir}/${_slint_archive}"
        STATUS _slint_download_status
        SHOW_PROGRESS)
    list(GET _slint_download_status 0 _slint_download_code)
    if(NOT _slint_download_code EQUAL 0)
        list(GET _slint_download_status 1 _slint_download_message)
        message(FATAL_ERROR "Download of Slint compiler binary package failed: ${_slint_download_message}")
    endif()
    file(ARCHIVE_EXTRACT INPUT "${_slint_download_dir}/${_slint_archive}" DESTINATION "${_slint_download_dir}")
    file(REMOVE "${_slint_download_dir}/${_slint_archive}")

    set(_slint_compiler_path "${_slint_download_dir}/slint-compiler${_slint_exe_suffix}")
    add_executable(Slint::slint-compiler IMPORTED GLOBAL)
    set_target_properties(Slint::slint-compiler PROPERTIES IMPORTED_LOCATION "${_slint_compiler_path}")
endif()

include(${slint_SOURCE_DIR}/api/cpp/cmake/SlintMacro.cmake)

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

# Ensure the generated Slint header exists before compiling its consumer.
set_source_files_properties(src/main.cpp PROPERTIES
    OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/appWindow.h"
)

# Local include directories (including CMAKE_CURRENT_BINARY_DIR for generated Slint headers)
target_include_directories(smart_hub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Library linking
target_compile_definitions(smart_hub PRIVATE PERSONAL_API_USE_PYBIND11=1)

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