cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Используем стандартный C++23, а не GNU-расширения языка.
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("nbe_chat"
        VERSION 1.0
        LANGUAGES CXX
)

SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
#
add_library(full_prj_warnings INTERFACE)
target_compile_options(full_prj_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
)
#

add_subdirectory("src/common")
add_subdirectory("src/server")
add_subdirectory("src/client")

#enable_testing()
#add_subdirectory("tests")
