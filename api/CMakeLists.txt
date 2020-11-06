cmake_minimum_required(VERSION 3.6)

project(libdcw)

add_definitions(-std=c++17)

if(MSVC OR MSYS OR MINGW)
    set(PLATFORM_DIR "windows")
endif()

if(UNIX AND NOT APPLE)
    set(PLATFORM_DIR "linux")
endif()

file(GLOB SOURCES
        "include/*.h"
        "src/*.cpp"
        "include/${PLATFORM_DIR}/*.h"
        "src/${PLATFORM_DIR}/*.cpp"
    )

add_library(${PROJECT_NAME}
  STATIC
    ${SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "include/private/${PLATFORM_DIR}")

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Threads::Threads)

target_compile_options(${PROJECT_NAME}
  PUBLIC
    -Wall -Wextra -Werror)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    LINKER_LANGUAGE CXX)
