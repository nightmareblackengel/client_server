cmake_minimum_required(VERSION 3.27)
SET(SERVER_PROJECT_NAME "nbe_chat_server")

SET (
        SERVER_CPP_FILES
        src/server/main.cpp
)

message(${SERVER_PROJECT_NAME})

project(${SERVER_PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
# ==========================================
# 1. КОНФИГУРАЦИЯ СЕРВЕРА
# ==========================================
add_executable(${SERVER_PROJECT_NAME})
target_sources(${SERVER_PROJECT_NAME} PRIVATE ${SERVER_CPP_FILES})
target_include_directories(${SERVER_PROJECT_NAME} PRIVATE "./include/")

