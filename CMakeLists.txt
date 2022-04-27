
cmake_minimum_required(VERSION 3.23)
project(vb6-script-2022)

add_executable(
    vb6-script-2022
    src/main.cpp
    src/class.cpp
    )

target_compile_features(
    vb6-script-2022
    PRIVATE
    cxx_std_17
    )
