// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>
#include "consts.hpp"

using namespace everest::modbus;

ModbusClient::ModbusClient(connection::Connection& conn_) : conn(conn_) {
    EVLOG(debug) << "Initialized ModbusClient";
}

const std::vector<uint8_t> ModbusClient::read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes) const {
    std::vector<uint8_t> body = utils::build_read_holding_register_message_body(first_register_address, num_registers_to_read);
    std::vector<uint8_t> full_message = full_message_from_body(body, consts::READ_HOLDING_REGISTER_MESSAGE_LENGTH, unit_id);
    conn.send_bytes(full_message);
    std::vector<uint8_t> response = conn.receive_bytes(consts::tcp::MAX_MESSAGE_SIZE);
    int num_register_bytes = validate_response(response, full_message);

    if (return_only_registers_bytes)
        return utils::extract_register_bytes_from_response(response, num_register_bytes);    
    
    return response;
}
