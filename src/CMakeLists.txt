cmake_minimum_required (VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-O2)

project (hid-inspector)

include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/vendor/include)

add_executable(
  hid-inspector
  src/main.cpp
  src/output_all_properties_json.cpp
  src/output_usages.cpp
)

target_link_libraries(
  hid-inspector
  "-framework CoreFoundation"
  "-framework IOKit"
)
