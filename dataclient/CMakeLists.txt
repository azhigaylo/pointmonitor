cmake_minimum_required(VERSION 3.1)

project(dataclient LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(Boost REQUIRED thread system)

qt5_wrap_cpp(moc_dataclient_src ${CMAKE_CURRENT_SOURCE_DIR}/include/dataclient/DataClientInterface.hpp)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataClientInterface.cpp
    ${moc_dataclient_src}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Boost::boost
    Boost::thread
    Boost::system
    )


