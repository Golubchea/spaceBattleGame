if(${PROJECT_NAME}_BUILD_EXAMPLES)
  file(GLOB_RECURSE EXAMPLE_SOURCES examples/*.cpp)

  # Для каждого .cpp файла создаем отдельный тест
  foreach(TEST_SOURCE ${EXAMPLE_SOURCES})
    # Извлекаем имя файла без расширения для использования в качестве имени теста
    get_filename_component(EX_NAME ${TEST_SOURCE} NAME_WE)

    # Создаем исполняемый файл для теста
    add_executable(${EX_NAME} ${TEST_SOURCE})

    set_target_properties(${EX_NAME} PROPERTIES
      INSTALL_RPATH $ORIGIN/../lib/:$ORIGIN
      BUILD_RPATH $ORIGIN/../lib/:$ORIGIN)

    # Связываем исполняемый файл с библиотеками Google Test и основным проектом
    target_link_libraries(${EX_NAME} PRIVATE
      "${PROJECT_NAME}::${PROJECT_NAME}"
    )
endforeach()

endif()
