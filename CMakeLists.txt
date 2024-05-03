cmake_minimum_required (VERSION 3.11)
 
project(yafth
  LANGUAGES CXX
  VERSION 0.0.1
)
 
add_executable(yafth src/main.cpp)
target_include_directories(yafth PRIVATE src)

set_target_properties(yafth PROPERTIES CXX_STANDARD 20)