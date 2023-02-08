# CMake version
cmake_minimum_required(VERSION 3.10)

# Declare project
project(fights)

# Set c++ standard
set(CMAKE_CXX_STANDARD 17)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# 3rd parties
include_directories(Libraries/doctest/doctest)
include_directories(Libraries/random/include)
include_directories(Libraries/json/single_include)

list(APPEND CMAKE_PREFIX_PATH "/usr/local/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Includes
include_directories(Includes)

# Project module
add_subdirectory(Sources/fights)
add_subdirectory(Sources/search)
add_subdirectory(Sources/selfplay)
add_subdirectory(Sources/selfviewer)
add_subdirectory(Sources/play)

add_subdirectory(Tests/UnitTests)
