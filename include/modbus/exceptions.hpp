// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_EXCEPTIONS_H
#define MODBUS_EXCEPTIONS_H

#include <stdexcept>

namespace everest { namespace modbus { namespace exceptions {
    
    class unmatched_response : public std::runtime_error {
        public:
            unmatched_response(const std::string& what_arg) : std::runtime_error(what_arg) {}
    };

  } // namespace exceptions
 } // namespace modbus
}; // namespace everest

#endif