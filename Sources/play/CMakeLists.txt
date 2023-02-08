# Target name
set(target play)

# Define
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Sources
file(GLOB_RECURSE sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

find_package(Threads REQUIRED)

add_executable(${target} ${sources})
include_directories(${LIBUUID_INCLUDE_DIR})
target_link_libraries(${target} fights search Threads::Threads ${TORCH_LIBRARIES})
