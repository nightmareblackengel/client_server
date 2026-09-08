cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Используем стандартный C++23, а не GNU-расширения языка.
set(CMAKE_CXX_EXTENSIONS OFF)

project("nbe_chat"
        VERSION 1.0
        LANGUAGES CXX
)
SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
#message(${SERVER_PROJECT_NAME})

# ==========================================
# 1. КОНФИГУРАЦИЯ СЕРВЕРА
# ==========================================
SET(SERVER_PROJECT_NAME "nbe_chat_server")
SET (SERVER_CPP_FILES src/server/main.cpp)

add_executable(${SERVER_PROJECT_NAME})
target_sources(${SERVER_PROJECT_NAME} PRIVATE ${SERVER_CPP_FILES})
target_include_directories(${SERVER_PROJECT_NAME} PRIVATE
        ${PROJECT_INCLUDE_DIR}
)

# ==========================================
# 2. КОНФИГУРАЦИЯ КЛИЕНТА
# ==========================================
SET (CLIENT_PROJECT_NAME "nbe_chat_client")
set (CLIENT_CPP_FILES src/client/main.cpp)

add_executable(${CLIENT_PROJECT_NAME})
target_sources(${CLIENT_PROJECT_NAME} PRIVATE ${CLIENT_CPP_FILES})
target_include_directories(${CLIENT_PROJECT_NAME} PRIVATE
        ${PROJECT_INCLUDE_DIR}
)
# ==========================================
# 3. Tests
# ==========================================
enable_testing()
add_subdirectory("./tests")
