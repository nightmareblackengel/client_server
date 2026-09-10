SET (CLIENT_PROJECT_NAME "nbe_chat_client")
set (CLIENT_CPP_FILES main.cpp)

add_executable(${CLIENT_PROJECT_NAME})
target_sources(${CLIENT_PROJECT_NAME} PRIVATE ${CLIENT_CPP_FILES})
target_include_directories(${CLIENT_PROJECT_NAME} PRIVATE
        "${PROJECT_SRC_DIR}/client/include"
)

target_link_libraries(${CLIENT_PROJECT_NAME} PRIVATE
        full_prj_warnings
        nbe_chat_common
)
