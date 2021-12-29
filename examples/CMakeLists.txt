# Modbus TCP example
add_executable(modbus_tcp_example modbus_tcp_client_example.cpp)
target_link_libraries(modbus_tcp_example everest::modbus everest::modbus::connection everest::log)

# Modbus UDP example
add_executable(modbus_udp_client_example modbus_udp_client_example.cpp)
target_link_libraries(modbus_udp_client_example everest::modbus everest::modbus::connection everest::log)

install(TARGETS modbus_tcp_example DESTINATION examples/modbus)
install(TARGETS modbus_udp_client_example DESTINATION examples/modbus)
