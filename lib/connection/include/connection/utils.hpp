// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

namespace everest { namespace connection { namespace utils {
    std::string get_bytes_hex_string(const std::vector<uint8_t>& bytes);
} } };
