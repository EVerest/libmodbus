add_library(connection)
add_library(modbus::connection ALIAS connection)
set_target_properties(connection PROPERTIES OUTPUT_NAME modbus_connection)
target_sources(connection
    PRIVATE
        src/rtu.cpp
        src/serial_connection_helper.cpp
        src/tcp.cpp
        src/udp.cpp
        src/utils.cpp
)

target_include_directories(connection
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(connection
    PRIVATE everest::log
)
