// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <vector>
#include <memory>

#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

int main() {
    everest::connection::TCPConnection conn("127.0.0.1", 502);
    if (!conn.is_valid()) return 0;
    everest::modbus::ModbusTCPClient client = everest::modbus::ModbusTCPClient(conn);
    std::vector<uint8_t> v = client.read_holding_register(1, 40000, 1);
    everest::modbus::utils::print_message_hex(v);
}