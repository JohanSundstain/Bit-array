cmake_minimum_required(VERSION 3.0)

project(bit_array)

enable_testing()

set(SOURCE main.cpp)
set(STATIC_LIB bit_array.cpp)

add_library(bit_array STATIC ${STATIC_LIB})
add_executable(main ${SOURCE})
target_link_libraries(main bit_array gtest gmock pthread)