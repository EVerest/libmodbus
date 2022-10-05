add_executable(tcp_connection_example connection_example.cpp)
target_link_libraries(tcp_connection_example
    PRIVATE modbus_connection
)

add_executable(udp_connection_example udp_connection_example.cpp)
target_link_libraries(udp_connection_example
    PRIVATE modbus_connection
)
