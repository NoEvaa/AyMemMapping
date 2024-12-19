cmake_minimum_required(VERSION 3.15)

# ==================================================
# Project

project(
    AyMemMapping
    VERSION 0.1.0
    LANGUAGES CXX
)

# Module: namespace::name
set(MODULE_NAME aymmap)
set(MODULE_NS ayin)

# ==================================================

# Project options
include(cmake/Options.cmake)
# Utils 
include(cmake/Utils.cmake)
# Init cmake environment
include(cmake/Init.cmake)

# ==================================================
# Module library

add_library(${MODULE_NAME} INTERFACE)
target_include_directories(${MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${MODULE_NS}::${MODULE_NAME} ALIAS ${MODULE_NAME})
message(STATUS "Build library `${MODULE_NS}::${MODULE_NAME}`")

# ==================================================
# Project development environment 

if(${PROJECT_NAME}_IS_MAIN)
    # Main
    add_subdirectory(main)

    # Tests
    if(${PROJECT_NAME}_ENABLE_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
    # Examples
    if(${PROJECT_NAME}_ENABLE_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()
