PROJECT(taskmon)
cmake_minimum_required(VERSION 2.6)

SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/.orogen/config")
INCLUDE(taskmonBase)

if (ROCK_TEST_ENABLED)
    enable_testing()
    find_package(Rock REQUIRED)
    add_subdirectory(test)
endif()
