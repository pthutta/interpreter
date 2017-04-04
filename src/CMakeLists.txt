add_library(headers INTERFACE)
target_include_directories(headers INTERFACE "${PROJECT_SOURCE_DIR}/src")
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ast.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/parser.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/scope.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/analyzer.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/call_stack.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/evaluator.h)
target_sources(headers INTERFACE ${SOURCE_FILES})

add_executable(Interpreter main.cpp)
target_link_libraries(Interpreter headers)