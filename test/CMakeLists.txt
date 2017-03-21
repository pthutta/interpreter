include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/files)
set(OUTPUT ${CMAKE_BINARY_DIR}/test/files)
file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/files/*.txt")

foreach(SRC ${TEST_FILES})
    string(REGEX REPLACE ".*\\/([^\\/]*)$" "\\1" FILE ${SRC})
    configure_file(${SOURCE}/${FILE} ${OUTPUT}/${FILE} COPYONLY)
endforeach()

set(UNIT_TEST Tests)
add_executable(${UNIT_TEST} tests.cpp catch.hpp parser_tests.cpp analyzer_tests.cpp)
target_link_libraries (${UNIT_TEST} headers)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
        COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "${UNIT_TEST}" --output-on-failure
)