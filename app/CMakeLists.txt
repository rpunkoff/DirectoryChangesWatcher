cmake_minimum_required(VERSION 3.6)

project(dcw CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall -Wextra -Werror")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Qml Quick QuickControls2 REQUIRED)

# enable QML debugging
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
#if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    add_definitions(-DQT_QML_DEBUG)
#endif()

#include qt resources
qt5_add_resources(APP_RESOURCES qml.qrc)

#qml dirs
set(QML_IMPORT_PATH  ${CMAKE_SOURCE_DIR}/app/qml CACHE STRING "" FORCE)
set(QML2_IMPORT_PATH ${CMAKE_SOURCE_DIR}/app/qml CACHE STRING "" FORCE)

include_directories(${API_SOURCE_DIR}/include)

file(GLOB APP_SOURCES
        "*.cpp"
        "include/*.h"
        "src/*.cpp")

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${APP_RESOURCES})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    libdcw
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
