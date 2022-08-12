// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <string>
#include <memory>
#include <sstream>

#include <connection/connection.hpp>
#include <connection/utils.hpp>

int main() {
    everest::connection::UDPConnection udp_conn("127.0.0.1", 502);
    std::vector<uint8_t> bytes({0x3F, 0x67, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x9C, 0x41, 0x00, 0x01});
    udp_conn.send_bytes(bytes);
    std::vector<uint8_t> received_bytes = udp_conn.receive_bytes(40);
    return 0;
}
