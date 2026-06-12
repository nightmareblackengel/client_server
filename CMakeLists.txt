cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 23)

SET(SERVER_PROJECT_NAME "nbe_chat_server")
SET (SERVER_CPP_FILES src/server/main.cpp)
project(${SERVER_PROJECT_NAME})

#message(${SERVER_PROJECT_NAME})

# ==========================================
# 1. КОНФИГУРАЦИЯ СЕРВЕРА
# ==========================================
add_executable(${SERVER_PROJECT_NAME})
target_sources(${SERVER_PROJECT_NAME} PRIVATE ${SERVER_CPP_FILES})
target_include_directories(${SERVER_PROJECT_NAME} PRIVATE
        "./include/"
)

# ==========================================
# 2. КОНФИГУРАЦИЯ КЛИЕНТА
# ==========================================
SET (CLIENT_PROJECT_NAME "nbe_chat_client")
set (CLIENT_CPP_FILES src/client/main.cpp)

#message(${CLIENT_PROJECT_NAME})

add_executable(${CLIENT_PROJECT_NAME})
target_sources(${CLIENT_PROJECT_NAME} PRIVATE ${CLIENT_CPP_FILES})
target_include_directories(${CLIENT_PROJECT_NAME} PRIVATE
        "./include/"
        "./include/exceptions"
        "./include/interfaces"
)
# ==========================================
# 3. Tests
# ==========================================
enable_testing()
add_subdirectory("./tests")
