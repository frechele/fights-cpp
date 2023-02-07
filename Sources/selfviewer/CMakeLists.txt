# Target name
set(target selfviewer)

# Define
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Sources
file(GLOB header_dir
    ${root_dir}/Includes/selfviewer)

file(GLOB_RECURSE sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_executable(${target} ${headers} ${sources})
target_link_libraries(${target} fights search)
