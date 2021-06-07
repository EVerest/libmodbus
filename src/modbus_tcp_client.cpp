#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

using namespace everest::modbus;

ModbusTCPClient::ModbusTCPClient(connection::Connection& conn_) : ModbusClient(conn_) {}

std::vector<uint8_t> ModbusTCPClient::full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) {
    // Creates and prepend MBAP header
    std::vector<uint8_t> mbap_header = utils::tcp::make_mbap_header(message_length, unit_id);
    std::vector<uint8_t> full_message;
    full_message.reserve( mbap_header.size() + body.size() );
    full_message.insert( full_message.end(), mbap_header.begin(), mbap_header.end() );
    full_message.insert( full_message.end(), body.begin(), body.end() );
    return full_message;
}

uint16_t ModbusTCPClient::validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) {
    return modbus::utils::tcp::check_mbap_header(request, response);
}