# ##############################################-tests
if(${PROJECT_NAME}_BUILD_TESTS)

  include(CTest)
  enable_testing()
  include(cmake/coverage.cmake)
  add_code_coverage() # Adds instrumentation to all targets

  target_code_coverage(${PROJECT_NAME} EXCLUDE 3rdparty)

  file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
  # Для каждого .cpp файла создаем отдельный тест
  foreach(TEST_SOURCE ${TEST_SOURCES})
    # Извлекаем имя файла без расширения для использования в качестве имени
    # теста
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Создаем исполняемый файл для теста
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    set_target_properties(
      ${TEST_NAME} PROPERTIES INSTALL_RPATH $ORIGIN/../lib/:$ORIGIN
                              BUILD_RPATH $ORIGIN/../lib/:$ORIGIN)

    # Связываем исполняемый файл с библиотеками Google Test и основным проектом
    target_link_libraries(
      ${TEST_NAME} PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}" gtest_main gtest
                           ${ADDED_LIBS2})

    target_include_directories(
      ${TEST_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:include>)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    target_code_coverage(${TEST_NAME} EXCLUDE 3rdparty)
  endforeach()
endif()
