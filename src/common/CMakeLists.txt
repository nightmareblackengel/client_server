SET (COMMON_PROJECT_NAME "nbe_chat_common")

add_library(${COMMON_PROJECT_NAME} INTERFACE)

target_include_directories(${COMMON_PROJECT_NAME} INTERFACE "${PROJECT_SRC_DIR}/common/include")
