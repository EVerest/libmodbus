#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

using namespace everest::modbus;

ModbusRTUClient::ModbusRTUClient(connection::Connection& conn_) : ModbusClient(conn_) {
    // TBD;
}

std::vector<uint8_t> ModbusRTUClient::full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) {
    // TBD
    return body;
}