cmake_minimum_required(VERSION 3.11)

project(modbus
    VERSION 0.1
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_TESTS "Build tests (gtest / test driven development)" OFF)
option(MODBUS_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(MODBUS_INSTALL OFF)
else()
    find_package(everest-log REQUIRED)

    if (BUILD_TESTS)
        find_package(GTest REQUIRED)
    endif()
endif()

add_subdirectory(lib/connection)

add_library(modbus)
add_library(everest::modbus ALIAS modbus)
target_sources(modbus
    PRIVATE
        src/modbus_client.cpp
        src/modbus_ip_client.cpp
        src/modbus_rtu_client.cpp
        src/utils.cpp
)

target_link_libraries(modbus
    PUBLIC
        modbus::connection
    PRIVATE
        everest::log
)

target_include_directories(modbus
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# packaging
if (MODBUS_INSTALL)
    install(
        TARGETS modbus connection
        EXPORT modbus-targets
    )

    install(
        DIRECTORY include/ lib/connection/include/
        TYPE INCLUDE
    )

    evc_setup_package(
        NAME everest-modbus
        NAMESPACE everest
        EXPORT modbus-targets
        ADDITIONAL_CONTENT
            "find_dependency(everest-log)"
    )
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib/connection/examples)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
