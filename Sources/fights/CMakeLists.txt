# Target name
set(target fights)

# Define
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Sources
file(GLOB header_dir
    ${root_dir}/Includes/fights)

file(GLOB_RECURSE headers
    ${header_dir}/*.hpp)

file(GLOB_RECURSE sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(${target} STATIC ${headers} ${sources})
