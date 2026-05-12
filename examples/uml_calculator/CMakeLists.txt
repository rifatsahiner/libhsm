cmake_minimum_required(VERSION 3.15)

execute_process(
    COMMAND fltk-config --cxxflags
    OUTPUT_VARIABLE FLTK_CXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND fltk-config --ldflags
    OUTPUT_VARIABLE FLTK_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REPLACE " " ";" FLTK_CXX_FLAGS_LIST "${FLTK_CXX_FLAGS}")

add_executable(uml_calculator main.cpp)

target_link_libraries(uml_calculator hsm ${FLTK_LDFLAGS})

target_compile_options(uml_calculator PRIVATE ${FLTK_CXX_FLAGS_LIST})
