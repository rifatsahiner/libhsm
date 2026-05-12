# UML Oven Example
add_executable(hsm_uml_oven_example main.cpp)

target_link_libraries(hsm_uml_oven_example PRIVATE hsm)
target_include_directories(hsm_uml_oven_example PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
