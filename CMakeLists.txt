cmake_minimum_required (VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(yafth
  VERSION 0.0.1
  LANGUAGES CXX
)

include_directories(include)
add_executable(yafth 
  src/main.cpp 
  src/yafth_engine.cpp
)

if(MSVC)
  target_compile_options(yafth PRIVATE /W4 /WX)
else()
  target_compile_options(yafth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()