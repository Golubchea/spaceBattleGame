include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

set(BOOST_INCLUDE_LIBRARIES system serialization asio stacktrace)
set(FETCHCONTENT_DOWNLOAD_DIR
    "${CMAKE_SOURCE_DIR}/3rdparty/downloads"
    CACHE PATH "Directory to download dependencies")

add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_ALL_DYN_LINK)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  GIT_PROGRESS TRUE
  DOWNLOAD_EXTRACT_TIMESTAMP ON)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
if(NOT googletest_POPULATED)
  FetchContent_MakeAvailable(googletest)
endif()

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  GIT_PROGRESS TRUE)
if(NOT boost_POPULATED)
  FetchContent_MakeAvailable(Boost)
  include_directories(${BOOST_LIBRARY_INCLUDES})
endif()

FetchContent_Declare(
  glfw
  URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
  URL_MD5 7d1e90c5c0011eb10eac280b6093d0d0
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  GIT_PROGRESS TRUE)
if(NOT glfw_POPULATED)
  FetchContent_MakeAvailable(glfw)
endif()

FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
  URL_MD5 78e541dadeee540ff9cedb5b618df821
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  GIT_PROGRESS TRUE)
if(NOT glm_POPULATED)
  FetchContent_MakeAvailable(glm)
endif()

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG 658f48e72aee3c6582e80b05ac0f8787a64fe6bb # v2.0.6
  GIT_PROGRESS TRUE
  SOURCE_SUBDIR cmake)

# if(NOT glad_POPULATED) FetchContent_MakeAvailable(glad)
# glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE LOADER API gl:core=4.3)
# endif()

# target_link_libraries(my_target PRIVATE glad_gl_core_43)

FetchContent_Declare(
  assimp
  URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.1.zip
  URL_MD5 baeb03e2aaed21a157a6343433f1509a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  GIT_PROGRESS TRUE)

set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)

if(BUILD_SHARED_LIBS)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(ASSIMP_BUILD_OBJ_IMPORTER
    ON
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL
    OFF
    CACHE BOOL "" FORCE)
if(NOT glad_POPULATED)
  FetchContent_MakeAvailable(assimp)
endif()

include_directories(${FETCHCONTENT_BASE_DIR}/assimp-src/include)
# target_include_directories(3d_renderer PRIVATE libs/assimp-src/include)
# target_link_libraries(3d_renderer assimp)
