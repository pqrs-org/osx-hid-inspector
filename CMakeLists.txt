cmake_minimum_required (VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-O2)

project (osx-iokit-inspector)

include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/vendor/include)

add_executable(
  osx-iokit-inspector
  src/main.cpp
)

target_link_libraries(
  osx-iokit-inspector
  "-framework CoreFoundation"
  "-framework IOKit"
)
