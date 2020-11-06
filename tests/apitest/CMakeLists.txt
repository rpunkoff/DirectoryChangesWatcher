cmake_minimum_required(VERSION 3.6)

project(libdcw_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)

include_directories(${API_SOURCE_DIR}/include)

enable_testing()

file(GLOB SOURCES
    "src/*.cpp"
    "src/*.h"
    "*.cpp"
    "*.h")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Test)
target_link_libraries(${PROJECT_NAME} libdcw)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

