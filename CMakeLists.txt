
cmake_minimum_required(VERSION 3.23)
project(vb6-script-2022)

add_library(
    vb6-script-2022-lib
    STATIC
    src/engine/classtype.cpp
    src/engine/function.cpp
    src/engine/value.cpp
    src/engine/context.cpp
    src/engine/functionbody.cpp
    src/engine/type.cpp
    src/engine/globalcontext.cpp
    src/engine/classinstance.cpp
    src/engine/module.cpp
    src/engine/array.cpp
    src/engine/valueorref.cpp
    src/code/lexer.cpp
    src/code/parser.cpp
    src/code/token.cpp
    src/code/parsetypes.cpp
    src/code/project.cpp
    src/code/codeblock.cpp
    src/libs/dx.cpp
    src/libs/simmodule.cpp
    src/libs/vbstdlib.cpp
    )

target_compile_features(
    vb6-script-2022-lib
    PUBLIC
    cxx_std_17
    )

target_include_directories(
    vb6-script-2022-lib
    PUBLIC
    src/engine
    src/code
    src
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


