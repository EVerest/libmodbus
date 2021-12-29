// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_UTILS_H
#define MODBUS_UTILS_H

#include <cstdint>
#include <vector>

#include "modbus_client.hpp"

namespace everest { namespace modbus { namespace utils {

    // General use utils
    std::vector<uint8_t> build_read_holding_register_message_body(uint16_t first_register_address, uint16_t num_registers_to_read);
    std::vector<uint8_t> extract_body_from_response(const std::vector<uint8_t>& response, int num_data_bytes);
    std::vector<uint8_t> extract_registers_bytes_from_response_body(const std::vector<uint8_t>& response_body);
    std::vector<uint8_t> extract_register_bytes_from_response(const std::vector<uint8_t>& response, int num_data_bytes);
    void print_message_hex(const std::vector<uint8_t>& message);
    void print_message_first_N_bytes(unsigned char *message, int N);

    // MODBUS/IP specific utils
    namespace ip {
        // Utility funcs
        std::vector<uint8_t> make_mbap_header(uint16_t message_length, uint8_t unit_id);
        uint16_t check_mbap_header(const std::vector<uint8_t>& sent_message, const std::vector<uint8_t>& received_message);
    }

  } // namespace utils
 }  // namespace modbus
};  // namespace everest

#endif
