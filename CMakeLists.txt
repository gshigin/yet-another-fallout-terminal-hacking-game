cmake_minimum_required (VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(yafth
  VERSION 0.0.3
  LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)
add_executable(yafth 
  src/main.cpp 
  src/engine.cpp
)

if(MSVC)
  target_compile_options(yafth PRIVATE /W4 /WX)
else()
  target_compile_options(yafth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()