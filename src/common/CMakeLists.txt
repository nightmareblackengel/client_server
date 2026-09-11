SET (COMMON_PROJECT_NAME "nbe_chat_common")

add_library(${COMMON_PROJECT_NAME} STATIC)

target_sources(${COMMON_PROJECT_NAME} PRIVATE library_test.cpp)

target_include_directories(${COMMON_PROJECT_NAME}
        PUBLIC
        "${PROJECT_SRC_DIR}/common/include"
)
