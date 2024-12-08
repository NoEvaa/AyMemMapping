# Utils
# 

# ==================================================
# Set the minimum C++ standard.
#
# ex.
#     SET_MINI_CXX_STANDARD(20)
#
macro(SET_MINI_CXX_STANDARD cxx_ver)
    if(DEFINED CMAKE_CXX_STANDARD)
        if(CMAKE_CXX_STANDARD LESS ${cxx_ver})
            set(CMAKE_CXX_STANDARD ${cxx_ver})
        endif()
    else()
        set(CMAKE_CXX_STANDARD ${cxx_ver})
    endif()
endmacro()

# ==================================================
# Get subdirectories of current path.
#
# ex.
#     get_subdirs(${CMAKE_CURRENT_SOURCE_DIR} subdirs)
#
function(get_subdirs cur_path out_dirs)
    set(out_list "")
    file(GLOB children LIST_DIRECTORIES true "${cur_path}/*")
    foreach(child IN LISTS children)
        if(IS_DIRECTORY ${child})
            list(APPEND out_list ${child})
        endif()
    endforeach()
    set(${out_dirs} ${out_list} PARENT_SCOPE)
endfunction()

