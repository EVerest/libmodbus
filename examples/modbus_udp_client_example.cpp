// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <memory>
#include <vector>

#include <connection/connection.hpp>
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

int main() {
    everest::connection::UDPConnection conn("127.0.0.1", 502);
    if (!conn.is_valid())
        return 0;
    everest::modbus::ModbusIPClient client(conn);
    std::vector<uint8_t> v = client.read_holding_register(1, 40000, 1);
    everest::modbus::utils::print_message_hex(v);
}
