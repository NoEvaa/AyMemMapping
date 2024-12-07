# ==================================================
# Unit tests

project(
    UnitTests
    LANGUAGES CXX
)

get_subdirs(${CMAKE_CURRENT_SOURCE_DIR} subdirs)
foreach(utest_path IN LISTS subdirs)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" utest_dir ${utest_path})
    
    # Test files
    file(GLOB_RECURSE utest_test_files
        CONFIGURE_DEPENDS
        "${utest_dir}/*.test.cc"
    )

    # Main files
    file(GLOB_RECURSE utest_main_files
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${utest_dir}/*.main.cc"
    )

    # Each main file will be compiled to an executable program
    foreach(main_file IN LISTS utest_main_files)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" main_rpath ${main_file})
        string(REPLACE ".main.cc" "" main_name ${main_rpath})
        string(REGEX REPLACE "/|\\.|\\\\" "_" main_name ${main_name})
        if(main_name STREQUAL "")
            continue()
        endif()

        message(STATUS "Build test executable: ${main_name}")
        
        # add executable
        add_executable(${main_name} ${main_file} ${utest_test_files})
        target_include_directories(
            ${main_name}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${utest_dir}
        )
        target_link_libraries(${main_name} PUBLIC tests::testlib)

        # add test
        add_test(
            NAME ${main_name}
            COMMAND $<TARGET_FILE:${main_name}>
        )
    endforeach()
endforeach()

