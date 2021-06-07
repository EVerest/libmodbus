cmake_minimum_required(VERSION 3.10)

# set the project name
project(everest_modbus VERSION 0.1)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/connection/)
include_directories(${PROJECT_SOURCE_DIR}/lib/connection/)

find_package(EDM REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${PROJECT_NAME} src/modbus_client.cpp src/modbus_tcp_client.cpp src/modbus_rtu_client.cpp src/utils.cpp)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        everest::log
        modbus_connection
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/lib/connection/include
)

add_executable(modbus_tcp_example examples/modbus_tcp_client_example.cpp)
target_link_libraries(modbus_tcp_example everest_modbus modbus_connection everest::log)
