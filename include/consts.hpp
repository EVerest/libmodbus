// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_CONSTS_H
#define MODBUS_CONSTS_H

#include <cstdint>

namespace everest {
namespace modbus {
namespace consts {

// General MODBUS constants
// TODO: this constant should have the value 5...?
constexpr uint16_t READ_REGISTER_COMMAND_LENGTH = 6;
constexpr uint8_t READ_HOLDING_REGISTER_FUNCTION_CODE = 3;
constexpr uint8_t READ_INPUT_REGISTER_FUNCTION_CODE = 4;

// MODBUS/RTU specific constants
namespace rtu {
constexpr uint16_t MAX_ADU = 256;
constexpr uint16_t MAX_PDU = 253;
constexpr uint16_t MAX_REGISTER_PER_MESSAGE = 125;
} // namespace rtu

// MODBUS/TCP specific constants
namespace tcp {
constexpr uint16_t MAX_ADU = 260;
constexpr uint16_t MAX_PDU = 253;
constexpr uint16_t PROTOCOL_ID = 0;
constexpr uint8_t MBAP_HEADER_LENGTH = 7;
constexpr uint16_t DEFAULT_PORT = 502;
} // namespace tcp

} // namespace consts
} // namespace modbus
}; // namespace everest

#endif
