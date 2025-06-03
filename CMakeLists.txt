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

# копирование ресурсов игры
# ##############################################################################
configure_file(data/spaceship.obj
               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/spaceship.obj)
configure_file(data/spaceship.mtl
               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/spaceship.mtl)

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

set(ADDED_LIBS
    boost_serialization
    boost_array
    boost_atomic
    boost_asio
    boost_align
    boost_assert
    boost_concept_check
    boost_config
    boost_context
    boost_coroutine
    boost_container_hash
    boost_core
    boost_detail
    boost_endian
    boost_function
    boost_function_types
    boost_fusion
    boost_integer
    boost_io
    boost_iterator
    boost_move
    boost_mp11
    boost_mpl
    boost_optional
    boost_phoenix
    boost_pool
    boost_predef
    boost_preprocessor
    boost_proto
    boost_range
    boost_smart_ptr
    boost_spirit
    boost_static_assert
    boost_thread
    boost_throw_exception
    boost_type_index
    boost_type_traits
    boost_typeof
    boost_unordered
    boost_utility
    boost_variant
    boost_bind
    boost_chrono
    boost_container
    boost_conversion
    boost_date_time
    boost_describe
    boost_exception
    boost_functional
    boost_regex
    boost_system
    boost_tuple
    boost_winapi
    boost_ratio
    boost_intrusive
    boost_algorithm
    boost_lexical_cast
    boost_numeric_conversion
    boost_tokenizer
    boost_variant2
    glm
    glm-header-only
    assimp)

add_subdirectory(libraries/Commands)
add_subdirectory(libraries/EntityComponentSystem)
add_subdirectory(libraries/Signals)
add_subdirectory(libraries/Network)
add_subdirectory(libraries/InversionOfControl)
add_subdirectory(libraries/BoundingVolumesHierarchy)
add_subdirectory(libraries/Visualization)
add_subdirectory(libraries/GameLogic)

# include(cmake/tests.cmake)

include(cmake/examples.cmake)

# include(cmake/install.cmake)
