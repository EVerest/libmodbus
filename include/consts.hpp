// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_CONSTS_H
#define MODBUS_CONSTS_H

#include <cstdint>

namespace everest { namespace modbus { namespace consts {

            // General MODBUS constants
            constexpr uint16_t READ_HOLDING_REGISTER_MESSAGE_LENGTH = 6;
            constexpr uint8_t READ_HOLDING_REGISTER_FUNCTION_CODE = 3;

            // TODO FIXME: the are different max sizes definded for modbus.
            // see: MODBUS APPLICATION PROTOCOL SPECIFICATION V1.1b3
            // page 3 and following.

            namespace rtu {
                constexpr uint16_t MAX_ADU = 256;
                constexpr uint16_t MAX_PDU = 253;
            }

            // MODBUS/TCP specific constants
            namespace tcp {
                constexpr uint16_t MAX_ADU = 260;
                constexpr uint16_t MAX_PDU = 253;
                constexpr uint16_t PROTOCOL_ID = 0;
                constexpr uint8_t MBAP_HEADER_LENGTH = 7;
                constexpr uint16_t DEFAULT_PORT = 502;
            }

        } // namespace consts
    }  // namespace modbus
};  // namespace everest

#endif
