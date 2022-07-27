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
            // read_holding_register needs to be virtual, since the rtu format differs from the ip/udp formats.
                virtual const std::vector<uint8_t> read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes=true) const;

        protected:

                const virtual std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const = 0;
                virtual uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) const = 0;

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

        // TODO: create rtu messaage class
        // TODO: create ABModbusClient

        using DataVectorUint16 = std::vector<std::uint16_t>;
        using DataVectorUint8  = std::vector<std::uint8_t>;

        enum struct ByteOrder { BigEndian, LittleEndian };

        class ModbusDataContainerUint16 {

        public:

            ModbusDataContainerUint16( ByteOrder byte_order, // which byteorder does the parameter payload has
                                       DataVectorUint16 payload ) :
                m_byte_order(byte_order),
                m_payload( payload )
                {}

            DataVectorUint8 get_payload_as_bigendian() const;

            std::size_t size() const { return m_payload.size(); }

        protected:

            ByteOrder m_byte_order;
            DataVectorUint16 m_payload;

        };

        class ModbusRTUClient : public ModbusClient {
            public:

            using MessageDataType = std::uint8_t;
            // using DataVector = std::vector<MessageDataType>;
            // using DataVectorUint16 = std::vector<std::uint16_t>;

            ModbusRTUClient(connection::Connection& conn_);
            virtual ~ModbusRTUClient() override;
            const DataVectorUint8 read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes = true  ) const override;

            bool writer_multiple_registers( uint8_t unit_id,
                                            uint16_t first_register_address,
                                            uint16_t num_registers_to_write ,
                                            const ModbusDataContainerUint16& payload
                );
            // TODO: check error code 0x90 in response

            static DataVectorUint8 response_without_protocol_data( const DataVectorUint8& raw_response, std::size_t payload_length );


        protected:

            const DataVectorUint8 full_message_from_body(const DataVectorUint8& body, uint16_t message_length, MessageDataType unit_id) const override;
            uint16_t validate_response(const DataVectorUint8& response, const DataVectorUint8& request) const override;
        };

    } // namespace modbus
};   // namespace everest
#endif
