add_library(modbus_connection)
add_library(modbus::connection ALIAS modbus_connection)
set_target_properties(modbus_connection PROPERTIES OUTPUT_NAME modbus_connection)
target_sources(modbus_connection
    PRIVATE
        src/rtu.cpp
        src/serial_connection_helper.cpp
        src/tcp.cpp
        src/udp.cpp
        src/utils.cpp
)

target_include_directories(modbus_connection
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(modbus_connection
    PRIVATE everest::log
)
