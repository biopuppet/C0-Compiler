cmake_minimum_required(VERSION 3.0.0)

add_executable(c0c
    ast.cpp
    ast_visitor.cpp
    debug.cpp
    error.cpp
    generator.cpp
    lexer.cpp
    main.cpp
    mips_isa.cpp
    parser.cpp
    quad_generator.cpp
    scope.cpp
    token.cpp
    type.cpp
)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_FLAGS "-g -Wall -fcolor-diagnostics -std=c++11")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)