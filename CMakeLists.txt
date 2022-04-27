
cmake_minimum_required(VERSION 3.23)
project(vb6-script-2022)

add_library(
    vb6-script-2022-lib
    STATIC
    src/class.cpp
    )

target_compile_features(
    vb6-script-2022-lib
    PUBLIC
    cxx_std_17
    )

add_executable(
    vb6-script-2022
    src/main/main.cpp
    )

target_link_libraries(
    vb6-script-2022
    PUBLIC
    vb6-script-2022-lib
    )

enable_testing()

add_subdirectory(test)


