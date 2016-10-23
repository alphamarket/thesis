cmake_minimum_required(VERSION 3.6)

SET(CMAKE_CXX_FLAGS "-std=c++14 -pthread")

project(THESIS)

file(GLOB_RECURSE ISOURCES "src/*.cpp")
file(GLOB_RECURSE IHEADER "inc/*.hpp" "inc/*.h")

set (IINCLUDE_DIRS "")
foreach (_headerFile ${IHEADER})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND IINCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES IINCLUDE_DIRS)

include_directories(${IINCLUDE_DIRS} "." "/usr/include/eigen3")

add_executable (thesis main.cpp main.helper.cpp ${ISOURCES})

target_link_libraries (thesis pthread boost_program_options)

# set warnings level
IF(MSVC) # MSVC compiler (Win32 only)
    # Display more warnings
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
ELSEIF(UNIX OR CMAKE_COMPILER_IS_GNUCXX) # Clang OR Gcc (Linux, Mac OS or Win32 with MingW)
    # displays all warnings
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
ENDIF()