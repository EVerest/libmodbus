add_executable(modbus_tcp_example modbus_tcp_client_example.cpp)
target_link_libraries(modbus_tcp_example
    PRIVATE modbus
)

add_executable(modbus_udp_client_example modbus_udp_client_example.cpp)
target_link_libraries(modbus_udp_client_example
    PRIVATE modbus
)
