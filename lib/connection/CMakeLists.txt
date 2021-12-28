cmake_minimum_required(VERSION 3.10)

# set the project name
project(connection VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(EDM REQUIRED)

add_library(libconnection src/connection.hpp src/tcp.cpp src/udp.cpp src/utils.cpp)
add_library(everest::modbus::connection ALIAS libconnection)

target_include_directories(libconnection
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(libconnection everest::log)
