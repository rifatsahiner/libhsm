# Examples CMakeLists.txt
option(HSM_BUILD_EXAMPLES "Build HSM examples" OFF)

if(HSM_BUILD_EXAMPLES)
    message(STATUS "Building examples")
    add_subdirectory(basic_usage)
    add_subdirectory(uml_oven)
    add_subdirectory(uml_hypothetical)
endif()
