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