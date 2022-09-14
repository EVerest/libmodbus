// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

using namespace everest::modbus;

ModbusIPClient::ModbusIPClient(connection::Connection& conn_) : ModbusClient(conn_) {
}

const std::vector<uint8_t> ModbusIPClient::full_message_from_body(const std::vector<uint8_t>& body,
                                                                  uint16_t message_length, uint8_t unit_id) const {
    // Creates and prepend MBAP header
    std::vector<uint8_t> mbap_header = utils::ip::make_mbap_header(message_length, unit_id);
    std::vector<uint8_t> full_message;
    full_message.reserve(mbap_header.size() + body.size());
    full_message.insert(full_message.end(), mbap_header.begin(), mbap_header.end());
    full_message.insert(full_message.end(), body.begin(), body.end());
    return full_message;
}

uint16_t ModbusIPClient::validate_response(const std::vector<uint8_t>& response,
                                           const std::vector<uint8_t>& request) const {
    return modbus::utils::ip::check_mbap_header(request, response);
}

ModbusTCPClient::ModbusTCPClient(connection::TCPConnection& conn_) : ModbusIPClient(conn_) {
}
ModbusUDPClient::ModbusUDPClient(connection::UDPConnection& conn_) : ModbusIPClient(conn_) {
}
