# ==================================================
# Project

project(
    TestLib
    LANGUAGES CXX
)

# ==================================================
# Module library

file(GLOB_RECURSE ${MODULE_NAME}_HEADERS
    CONFIGURE_DEPENDS
    "include/*.h" "include/*.hpp"
    "include/*.inl" "include/*.tcc"
)

file(GLOB_RECURSE ${MODULE_NAME}_SOURCES
    CONFIGURE_DEPENDS
    "src/*.cc" "src/*.cpp" "src/*.c"
    "src/*.h" "src/*.hpp"
    "src/*.inl" "src/*.tcc"
)

add_library(
    testlib
    STATIC
    ${${MODULE_NAME}_HEADERS}
    ${${MODULE_NAME}_SOURCES}
)

target_include_directories(
    testlib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(testlib PUBLIC ${MODULE_NS}::${MODULE_NAME})

add_library(tests::testlib ALIAS testlib)

