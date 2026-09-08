SET(SERVER_PROJECT_NAME "nbe_chat_server")
SET (SERVER_CPP_FILES main.cpp)

add_executable(${SERVER_PROJECT_NAME})
target_sources(${SERVER_PROJECT_NAME} PRIVATE ${SERVER_CPP_FILES})
target_include_directories(${SERVER_PROJECT_NAME} PRIVATE
        ${PROJECT_INCLUDE_DIR}
)

target_link_libraries(${SERVER_PROJECT_NAME} PRIVATE full_prj_warnings)
