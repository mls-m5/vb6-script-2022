cmake_minimum_required(VERSION 3.23)
project(vb6-script-2022)

enable_testing()

function(test_disabled testname)
    add_executable(${testname}_test ${testname}_test.cpp)

    target_include_directories(${testname}_test PRIVATE tests)

    target_compile_features(
        ${testname}_test
        PRIVATE
        cxx_std_17
        )

    target_link_libraries(
        ${testname}_test
        PUBLIC
        vb6-script-2022-lib
        )

    target_include_directories(
        ${testname}_test
        PUBLIC
        )

endfunction()

function(test testname)
    test_disabled(${testname})

    add_test(NAME ${testname}_test
        COMMAND ${testname}_test
        )
endfunction()

file(COPY scripts DESTINATION .)
file(COPY old-projects/ DESTINATION .)

test(class)
test(function)
test(lexer)
test(parser)
test(script)
test(struct)
test(array)
test_disabled(project)
