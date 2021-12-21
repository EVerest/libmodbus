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
#ifndef MODBUS_CLIENT_H
#define MODBUS_CLIENT_H

#include <memory>
#include <cstdint>
#include <vector>

#include <connection/tcp.hpp>

namespace everest { namespace modbus {

        class ModbusClient {
            public:
                ModbusClient(connection::Connection& conn_);
                virtual ~ModbusClient() {};
                std::vector<uint8_t> read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes=true);
                virtual std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) = 0;
                virtual uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) = 0;

            private:
                connection::Connection& conn;
        };

        class ModbusTCPClient : public ModbusClient {
            public:
                ModbusTCPClient(connection::Connection& conn_);
                virtual ~ModbusTCPClient() {};
                std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) override;
                uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) override;
        };

        class ModbusRTUClient : public ModbusClient {
            public:
                ModbusRTUClient(connection::Connection& conn_);
                virtual ~ModbusRTUClient() {};
                std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id);
        };

    } // namespace modbus
};   // namespace everest
#endif