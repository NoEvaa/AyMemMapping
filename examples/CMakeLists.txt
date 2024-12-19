# ==================================================
# Examples

project(
    Examples
    LANGUAGES CXX
)

get_subdirs(${CMAKE_CURRENT_SOURCE_DIR} subdirs)
foreach(ex_path IN LISTS subdirs)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" ex_dir ${ex_path})
    
    # Main files
    file(GLOB_RECURSE ex_main_files
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${ex_dir}/*.ex.cc"
    )

    # Each main file will be compiled to an executable program
    foreach(main_file IN LISTS ex_main_files)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" main_rpath ${main_file})
        string(REPLACE ".ex.cc" "" main_name ${main_rpath})
        string(REGEX REPLACE "/|\\.|\\\\" "_" main_name ${main_name})
        if(main_name STREQUAL "")
            continue()
        endif()

        message(STATUS "Build example executable: ${main_name}")
        
        # add executable
        add_executable(${main_name} ${main_file})
        target_include_directories(
            ${main_name}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${ex_dir}
        )
        target_link_libraries(${main_name} PUBLIC ${MODULE_NS}::${MODULE_NAME})
    endforeach()
endforeach()

