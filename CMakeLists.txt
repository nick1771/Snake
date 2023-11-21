cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    BasicSnake
    VERSION 0.1.0
    DESCRIPTION "Basic snake"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    BasicSnake_lib OBJECT
    source/BitmapData.cpp
    source/BitmapRenderer.cpp
    source/Random.cpp
    source/Rectangle.cpp
    source/Window.cpp
)

target_include_directories(
    BasicSnake_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(BasicSnake_lib PUBLIC cxx_std_17)

add_compile_definitions(UNICODE=1)

# ---- Declare executable ----

add_executable(BasicSnake_exe source/main.cpp)
add_executable(BasicSnake::exe ALIAS BasicSnake_exe)

set_property(TARGET BasicSnake_exe PROPERTY OUTPUT_NAME BasicSnake)

target_compile_features(BasicSnake_exe PRIVATE cxx_std_17)

target_link_libraries(BasicSnake_exe PRIVATE BasicSnake_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT BasicSnake_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of BasicSnake"
  )
endif()

include(cmake/dev-mode.cmake)
