/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MODBUS_CONSTS_H
#define MODBUS_CONSTS_H

#include <cstdint>

namespace everest { namespace modbus { namespace consts {

    // General MODBUS constants
    const uint16_t READ_HOLDING_REGISTER_MESSAGE_LENGTH = 6;
    const uint8_t READ_HOLDING_REGISTER_FUNCTION_CODE = 3;

    // MODBUS/TCP specific constants
    namespace tcp {
        const uint16_t PROTOCOL_ID = 0;
        const uint8_t MBAP_HEADER_LENGTH = 7;
        const uint16_t DEFAULT_PORT = 502;
        const uint16_t MAX_MESSAGE_SIZE = 256;
    }

  } // namespace consts
 }  // namespace modbus
};  // namespace everest

#endif