cmake_minimum_required(VERSION 3.6)

project(DirectoryChangesWatcher CXX)

set(API_SOURCE_DIR "${CMAKE_SOURCE_DIR}/api")

add_subdirectory(api)
add_subdirectory(tests)
add_subdirectory(app)
