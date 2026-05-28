cmake_minimum_required(VERSION 3.27)
SET(PROJECT_NAME "chat_server")

SET (
        CPP_FILES
        src/main.cpp
)

#message(${PROJECT_NAME})
add_executable(${PROJECT_NAME})
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)

target_sources(${PROJECT_NAME} PRIVATE ${CPP_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "./include/")

