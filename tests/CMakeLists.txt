include(FetchContent)
SET (TESTS_PROJECT_NAME "nbe_tests")

# 1. Указываем, откуда скачать Google Test и какую версию (тег) использовать
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
)

# Для Windows-пользователей: предотвращает переопределение настроек рантайма (gtest_force_shared_crt)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# 2. Делаем пакет доступным для сборки
FetchContent_MakeAvailable(googletest)
# 3. Создаем исполняемый файл для тестов
add_executable(
    ${TESTS_PROJECT_NAME}
    MyQueueTest.cpp
)
# 4. Связываем наши тесты с библиотеками Google Test
# (gtest_main автоматически добавляет функцию main для тестов)
target_link_libraries(
    ${TESTS_PROJECT_NAME}
    PRIVATE gtest_main
)
# 5. Интегрируем тесты с утилитой CTest (чтобы можно было запускать через 'ctest' или вкладку в CLion)
include(GoogleTest)
gtest_discover_tests(${TESTS_PROJECT_NAME})
