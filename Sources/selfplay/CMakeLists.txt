# Target name
set(target selfplay)

# Define
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Sources
file(GLOB header_dir
    ${root_dir}/Includes/selfplay)

file(GLOB_RECURSE headers
    ${header_dir}/*.hpp)

file(GLOB_RECURSE sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

find_package(Threads REQUIRED)

find_path(LIBUUID_INCLUDE_DIR uuid.h PATH_SUFFIXES uuid)
find_library(LIBUUID_LIBRARY libuuid.a)

add_executable(${target} ${headers} ${sources})
include_directories(${LIBUUID_INCLUDE_DIR})
target_link_libraries(${target} fights search Threads::Threads ${LIBUUID_LIBRARY})
