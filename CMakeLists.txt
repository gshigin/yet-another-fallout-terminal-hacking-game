cmake_minimum_required (VERSION 3.21)

project(yafth
  VERSION 0.4.2
  LANGUAGES CXX
)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

option(YAFTH_CLANG_TIDY "Set to ON to use clang tidy" OFF)

# Dependencies 

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Build
file(GLOB_RECURSE LIB_SOURCES
    src/app/*.cpp
    src/core/*.cpp
    src/ui/*.cpp
    src/util/*.cpp
)

add_library(${PROJECT_NAME}-lib ${LIB_SOURCES})
target_include_directories(${PROJECT_NAME}-lib 
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}-lib 
  PRIVATE ftxui::component
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE ${PROJECT_NAME}-lib
)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )
if (YAFTH_CLANG_TIDY)
  if (NOT CLANG_TIDY_EXE)
    message(FATAL_ERROR "clang-tidy requested but executable not found")
  endif()

set_target_properties(${PROJECT_NAME}-lib
  PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
)
endif()

if(MSVC)
  target_compile_options(yafth PRIVATE /W4 /WX)
else()
  target_compile_options(yafth PRIVATE -Wall -Wextra -Wpedantic -Werror -Wdeprecated)
endif()