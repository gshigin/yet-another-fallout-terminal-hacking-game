cmake_minimum_required(VERSION 3.14)
project(yafth VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
  GIT_TAG v7.0.3)
FetchContent_MakeAvailable(ftxui)

add_executable(yafth main.cpp)
target_compile_features(yafth PRIVATE cxx_std_17)
set_target_properties(yafth PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(yafth PRIVATE ftxui::component)

if(MSVC)
  target_compile_options(yafth PRIVATE /W4 /WX /permissive-)
else()
  target_compile_options(yafth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

option(YAFTH_CLANG_TIDY "Run clang-tidy on the example" OFF)
if(YAFTH_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)
  if(NOT CLANG_TIDY_EXE)
    message(FATAL_ERROR "YAFTH_CLANG_TIDY requires clang-tidy")
  endif()
  set_target_properties(yafth PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
  if(MSVC)
    set_property(TARGET yafth APPEND PROPERTY CXX_CLANG_TIDY
      --extra-arg=-Xclang --extra-arg=-fexceptions --extra-arg=-Xclang --extra-arg=-fcxx-exceptions)
  endif()
endif()
