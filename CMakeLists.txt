cmake_minimum_required(VERSION 3.10)
project(Project2147 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(main)
target_sources(main PRIVATE
    main.cpp
    placesDB.cpp
    validation.cpp
    placesApp.cpp
)

target_compile_options(main PRIVATE -Wall -Wextra -Werror)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(main PRIVATE -Wpedantic)
endif()

target_link_libraries(main PRIVATE
    Threads::Threads
    sqlite3
)

add_custom_target(format
    COMMAND clang-format -i *.cpp *.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting source files..."
)

add_custom_target(run
    COMMAND ./main
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS main
    COMMENT "Running application..."
)
