// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_CLIENT_H
#define MODBUS_CLIENT_H

#include <memory>
#include <cstdint>
#include <vector>

#include <connection/connection.hpp>

namespace everest { namespace modbus {

        class ModbusClient {
            public:
                ModbusClient(connection::Connection& conn_);
                virtual ~ModbusClient() = default;
                const std::vector<uint8_t> read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes=true) const;
                const virtual std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const = 0;
                virtual uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) const = 0;

            private:
                ModbusClient(const ModbusClient&) = delete;
                ModbusClient& operator=(const ModbusClient&) = delete;
                connection::Connection& conn;
        };

        class ModbusIPClient : public ModbusClient {
            public:
                ModbusIPClient(connection::Connection& conn_);
                virtual ~ModbusIPClient() = default;
                const std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const override;
                uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) const override;
        };

        class ModbusTCPClient : public ModbusIPClient {
            public:
                ModbusTCPClient(connection::TCPConnection& conn_);
                ~ModbusTCPClient() override = default;
        };

        class ModbusUDPClient : public ModbusIPClient {
            public:
                ModbusUDPClient(connection::UDPConnection& conn_);
                ~ModbusUDPClient() override = default;
        };

        class ModbusRTUClient : public ModbusClient {
            public:
                ModbusRTUClient(connection::Connection& conn_);
                virtual ~ModbusRTUClient() {};
                const std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const override;
        };

    } // namespace modbus
};   // namespace everest
#endif
