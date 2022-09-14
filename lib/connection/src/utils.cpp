// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <connection/utils.hpp>

using namespace everest::connection;

std::string utils::get_bytes_hex_string(const std::vector<uint8_t>& bytes) {
    std::stringstream buffer;
    for (auto it = bytes.begin(); it != bytes.end(); it++) {
        buffer << std::hex << (int)*it << " ";
    }
    return buffer.str();
}
