cmake_minimum_required(VERSION 3.21)
project(smart_hub CXX)

# set C++ standard ro C++20 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flag defaults
if(NOT CMAKE_BUILD-TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# Dependencies (Fetched or Found)
