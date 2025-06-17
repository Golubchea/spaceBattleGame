# Указываем правила установки библиотеки
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} ${ADDED_LIBS}
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
