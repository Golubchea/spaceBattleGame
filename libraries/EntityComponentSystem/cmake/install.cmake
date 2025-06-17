# Указываем правила установки библиотеки
include(GNUInstallDirs)
install(
  TARGETS ${PROJECT_NAME} ${ADDED_LIBS}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # Установка библиотеки
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Установка исполняемых файлов
                                              # (если есть)
)

# Устанавливаем заголовочные файлы
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)# Копируем заголовочные файлы в системную директорию include

if(${PROJECT_NAME}_BUILD_EXAMPLES)
  file(GLOB_RECURSE EXAMPLE_SOURCES examples/*.cpp)

  # Для каждого .cpp файла создаем отдельный тест
  foreach(TEST_SOURCE ${EXAMPLE_SOURCES})
    # Извлекаем имя файла без расширения для использования в качестве имени
    # теста
    get_filename_component(EX_NAME ${TEST_SOURCE} NAME_WE)

    install(
      TARGETS ${EX_NAME} ${ADDED_LIBS}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # Установка библиотеки
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Установка исполняемых файлов
                                                  # (если есть)
    )
  endforeach()

endif()

if(${PROJECT_NAME}_BUILD_TESTS)

  file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
  # Для каждого .cpp файла создаем отдельный тест
  foreach(TEST_SOURCE ${TEST_SOURCES})
    # Извлекаем имя файла без расширения для использования в качестве имени
    # теста
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    install(
      TARGETS ${TEST_NAME} ${ADDED_LIBS}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # Установка библиотеки
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Установка исполняемых файлов
                                                  # (если есть)
    )
  endforeach()
endif()

# ##############################################-tests
# if(${PROJECT_NAME}_BUILD_TESTS)

# file(GLOB_RECURSE TEST_SOURCES tests/*.cpp) # Для каждого .cpp файла создаем
# отдельный тест foreach(TEST_SOURCE ${TEST_SOURCES}) # Извлекаем имя файла без
# расширения для использования в качестве имени # теста
# get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

# # Создаем исполняемый файл для теста add_executable(${TEST_NAME}
# ${TEST_SOURCE})

# endforeach() endif()

# Экспортируем целевой объект для использования в других проектах
install(
  EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
