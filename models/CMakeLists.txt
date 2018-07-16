cmake_minimum_required(VERSION 3.1)

project(datamodels LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(Boost REQUIRED thread system)

qt5_wrap_cpp(moc_datamodels_src ${CMAKE_CURRENT_SOURCE_DIR}/include/models/DataModels.hpp)
qt5_wrap_cpp(moc_conditions_src ${CMAKE_CURRENT_SOURCE_DIR}/include/models/Conditions.hpp)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataModels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Conditions.cpp
    ${moc_datamodels_src}
    ${moc_conditions_src}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    dataclient
    )


