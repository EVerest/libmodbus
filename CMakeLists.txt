cmake_minimum_required(VERSION 3.10)

# set the project name
project(modbus VERSION 0.1)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/connection/)
include_directories(${PROJECT_SOURCE_DIR}/lib/connection/)

find_package(EDM REQUIRED)

# specify the C++ standard
# set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_link_options(-fuse-ld=mold)

option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_TDD_STUFF "Build gtest tdd stuff" OFF)
# option(BUILD_SHARED_LIBS "Build shared libs" ON)

add_library(${PROJECT_NAME} src/modbus_client.cpp src/modbus_ip_client.cpp src/modbus_rtu_client.cpp src/utils.cpp)
add_library(everest::modbus ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        everest::log
        everest::modbus::connection
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/lib/connection/include
)

if (BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib/connection/examples)
endif()

if (BUILD_TDD_STUFF)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
