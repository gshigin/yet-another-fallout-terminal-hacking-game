cmake_minimum_required (VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(yafth
  VERSION 0.1.0
  LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Hack for now, need good macros for exports later
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${PROJECT_NAME} SHARED
  src/engine.cpp
)
add_library(${PROJECT_NAME}::engine ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
  GIT_PROGRESS TRUE
  GIT_SHALLOW FALSE
  EXCLUDE_FROM_ALL
  FIND_PACKAGE_ARGS NAMES ftxui
)

FetchContent_GetProperties(ftxui)
if(ftxui_POPULATED)
  return()
endif()

FetchContent_MakeAvailable(ftxui)

add_executable(app main.cpp)
target_link_libraries(app PUBLIC yafth::engine)

target_link_libraries(app
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  target_compile_options(yafth PRIVATE /W4 /WX)
else()
  target_compile_options(yafth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()