cmake_minimum_required(VERSION 3.10)

# set the project name
project(connection VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# option(BUILD_SHARED_LIBS "Build shared libs" ON)

find_package(EDM REQUIRED)

add_library(libconnection include/connection/connection.hpp src/tcp.cpp src/udp.cpp src/utils.cpp src/rtu.cpp src/serial_connection_helper.cpp )
add_library(everest::modbus::connection ALIAS libconnection)

target_include_directories(libconnection
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(libconnection everest::log)
