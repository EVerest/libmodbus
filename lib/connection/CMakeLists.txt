cmake_minimum_required(VERSION 3.10)

# set the project name
project(modbus_connection VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(EDM REQUIRED)

add_library(${PROJECT_NAME} src/connection.hpp src/tcp.cpp src/utils.cpp)
target_link_libraries(${PROJECT_NAME} everest::log)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
)

add_executable(${PROJECT_NAME}_example ${PROJECT_SOURCE_DIR}/examples/connection_example.cpp)
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME} everest::log)
