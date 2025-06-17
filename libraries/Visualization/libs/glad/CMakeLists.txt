cmake_minimum_required(VERSION 3.16)

# Получаем имя проекта из имени директории
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} VERSION 1.0.0)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Собираем исходные файлы
file(GLOB_RECURSE SRC src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS include/*.h) # Добавляем заголовочные файлы

# Создаем библиотеку
add_library(${PROJECT_NAME} ${SRC} ${HEADERS}) # Включаем заголовочные файлы в библиотеку
# target_link_libraries(${PROJECT_NAME} PUBLIC Interfaces::Interfaces )
# Добавляем директорию include для заголовочных файлов
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Создаем алиас для библиотеки
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

# Указываем правила установки библиотеки
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # Установка библиотеки
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Установка исполняемых файлов (если есть)
)

# Устанавливаем заголовочные файлы
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # Копируем заголовочные файлы в системную директорию include

# Экспортируем целевой объект для использования в других проектах
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
