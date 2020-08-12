cmake_minimum_required(VERSION 3.15)
project(CherryEngine VERSION 1.1 LANGUAGES CXX)

# --- project settings
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CHERRY_ENGINE_ENABLE_CACHE "Enable cache if avaible" ON)
option(CHERRY_ENGINE_ENABLE_IPO "Enable Interprocedural Optimalization" ON)
option(CHERRY_ENGINE_ENABLE_PCH "Enable precompiled headers" ON)
option(CHERRY_ENGINE_BUILD_WARNINGS "Show build warning" ON)
option(CHERRY_ENGINE_BUILD_SHARED "Build shared library" ON)
option(CHERRY_ENGINE_BUILD_TESTS "Build tests" ON)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CHERRY_ENGINE_MASTER_PROJECT ON)
endif()
option(CHERRY_ENGINE_INSTALL "Install library" ${CHERRY_ENGINE_MASTER_PROJECT})

# --- build project
message(STATUS "Building ${PROJECT_NAME}: ${PROJECT_VERSION}")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWIthDebInfo" CACHE STRING "Choose the type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWIthDebInfo")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE sources "src/*.cpp" "src/*.h")

if(CHERRY_ENGINE_BUILD_SHARED)
  add_library(CherryEngine SHARED "${sources}")
else()
  add_library(CherryEngine STATIC "${sources}")
endif()
target_include_directories(CherryEngine PUBLIC
  "${CMAKE_CURRENT_BINARY_DIR}"
  "include"
  "src")
target_compile_features(CherryEngine PRIVATE cxx_std_17)
set_target_properties(CherryEngine PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MINOR})

include(GenerateExportHeader)
generate_export_header(CherryEngine
  BASE_NAME CHERRY
  EXPORT_FILE_NAME CherryExport.h)

# --- build options
include(utils)
enable_warnings(CherryEngine)

if(CHERRY_ENGINE_ENABLE_CACHE)
  set(CHERRY_ENGINE_CACHE_OPTION "ccache" CACHE STRING "Compiler cache to be used")
  find_program(CHERRY_ENGINE_CACHE_BINARY ${CHERRY_ENGINE_CACHE_OPTION})
  if(CHERRY_ENGINE_CACHE_BINARY)
    message(STATUS "${CACHE_OPTION} found and enabled.")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CHERRY_ENGINE_CACHE_BINARY})
  else()
    message(STATUS "${CACHE_OPTION} is enabled but was not found. Not using it.")
  endif()
endif()

if(CHERRY_ENGINE_ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT error)
  if(ipo_supported)
    message(STATUS "Interprocedural Optimalization enabled.")
    set_target_properties(CherryEngine PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(STATUS "Interprocedural Optimalization not supported: ${error}")
  endif()
endif()

if(CHERRY_ENGINE_ENABLE_PCH)
  target_precompile_headers(CherryEngine PRIVATE "cmake/pch.h")
endif()

if(CHERRY_ENGINE_BUILD_TESTS)
  message(STATUS "Building tests")
  enable_testing()
  add_subdirectory(tests)
endif()

# --- dependencies
set(OpenGL_GL_PREFERENCE LEGACY)

add_subdirectory(deps/glfw             glfw    EXCLUDE_FROM_ALL)
add_subdirectory(deps/glm              glm     EXCLUDE_FROM_ALL)
add_subdirectory(deps/spdlog           spdlog  EXCLUDE_FROM_ALL)
add_subdirectory(deps/glew/build/cmake glew    EXCLUDE_FROM_ALL)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "deps/stb")

find_package(OpenGL REQUIRED)

target_link_libraries(CherryEngine
  PUBLIC
  glfw
  glew
  glm
  stb
  spdlog
  OpenGL::OpenGL)

# --- installation
if(CHERRY_ENGINE_INSTALL)
  include(GNUInstallDirs)
  install(TARGETS CherryEngine
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
endif()
