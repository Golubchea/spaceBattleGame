# ##############################################################################

cmake_minimum_required(VERSION 3.16)

# Получаем имя проекта из имени директории
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/archives)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ##############################################################################

option(${PROJECT_NAME}_BUILD_EXAMPLES "build examples" ON)
option(${PROJECT_NAME}_BUILD_TESTS "build tests" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# ##############################################################################

include(cmake/fetch.cmake)
include_directories(${PROJECT_SOURCE_DIR}/include)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fprofile-arcs
# -ftest-coverage") set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g
# -fprofile-arcs -ftest-coverage")

# ##############################################################################
include_directories(libs/jwt-cpp/include)
add_subdirectory(libs/jwt-cpp)

# Собираем исходные файлы
file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp) # Добавляем заголовочные
                                                     # файлы

# Создаем библиотеку
if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SRC} ${HEADERS})
else()
  add_library(${PROJECT_NAME} STATIC ${SRC} ${HEADERS})
endif()

set_target_properties(
  ${PROJECT_NAME} PROPERTIES INSTALL_RPATH $ORIGIN/../lib/:$ORIGIN
                             BUILD_RPATH $ORIGIN/../lib/:$ORIGIN)

# Добавляем директорию include для заголовочных файлов
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)

# Создаем алиас для библиотеки
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

set(ADDED_LIBS2
    jwt-cpp::jwt-cpp
    Signals::Signals
    Network::Network
    Commands::Commands
    EntityComponentSystem::EntityComponentSystem
    InversionOfControl::InversionOfControl
    BoundingVolumesHierarchy::BoundingVolumesHierarchy
    Visualization::Visualization
    glad::glad
    assimp
    glfw)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Boost::asio Boost::system Boost::core
                          Boost::serialization ${ADDED_LIBS2})

include(cmake/tests.cmake)

include(cmake/examples.cmake)

include(cmake/install.cmake)
