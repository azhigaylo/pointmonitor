cmake_minimum_required(VERSION 3.1)

project(dataclient LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Network)
find_package(Boost REQUIRED COMPONENTS thread system)

if(Boost_FOUND)
    message("Boost_FOUND !!!!")
    message("Boost_INCLUDE_DIRS = '${Boost_INCLUDE_DIRS}'")
    message("Boost_LIBRARIES = '${Boost_LIBRARIES}'")
else()
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

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
