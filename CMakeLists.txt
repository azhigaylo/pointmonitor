cmake_minimum_required(VERSION 3.1)

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(pointmonitor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Widgets REQUIRED)

add_subdirectory(dataclient)
add_subdirectory(models)

add_executable(${PROJECT_NAME} "main.cpp" "qml.qrc")

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Quick
    dataclient
    datamodels
    )
