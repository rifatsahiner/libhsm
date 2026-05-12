# Basic Usage Example
add_executable(hsm_basic_example main.cpp)

target_link_libraries(hsm_basic_example PRIVATE hsm)
target_include_directories(hsm_basic_example PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)