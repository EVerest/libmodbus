// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_CLIENT_H
#define MODBUS_CLIENT_H

#include <memory>
#include <cstdint>
#include <vector>

#include <connection/connection.hpp>
#include <consts.hpp>

namespace everest { namespace modbus {

        class ModbusClient {
        public:
            ModbusClient(connection::Connection& conn_);
            virtual ~ModbusClient() = default;
            // read_holding_register needs to be virtual, since the rtu format differs from the ip/udp formats.
            virtual const std::vector<uint8_t> read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes=true) const;

        protected:

            // TODO: what to do with this method?
            const virtual std::vector<uint8_t> full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const = 0;

            virtual uint16_t validate_response(const std::vector<uint8_t>& response, const std::vector<uint8_t>& request) const = 0;

            // message size including protocol data (addressing, error check, mbap)
            virtual std::size_t max_adu_size() const = 0;
            // message size without protocol data (addressing, error check, mbap), function code and payload data only
            virtual std::size_t max_pdu_size() const = 0;

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
            // message size including protocol data (addressing, error check, mbap)
            virtual std::size_t max_adu_size() const override { return everest::modbus::consts::tcp::MAX_ADU; }
            // message size without protocol data (addressing, error check, mbap), function code and payload data only
            virtual std::size_t max_pdu_size() const override { return everest::modbus::consts::tcp::MAX_PDU; }

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

            ModbusRTUClient(connection::Connection& conn_);
            virtual ~ModbusRTUClient() override;

            // throws std::runtime_error
            const DataVectorUint8 read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes = true  ) const override;

            // throws std::runtime_error
            DataVectorUint8 writer_multiple_registers( uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_write , const ModbusDataContainerUint16& payload, bool return_only_registers_bytes ) const; // errors will be reported by exception std::runtime_error

            // message size including protocol data (addressing, error check, mbap)
            virtual std::size_t max_adu_size() const override { return everest::modbus::consts::rtu::MAX_ADU; }
            // message size without protocol data (addressing, error check, mbap), function code and payload data only
            virtual std::size_t max_pdu_size() const override { return everest::modbus::consts::rtu::MAX_PDU; }

            static DataVectorUint8 response_without_protocol_data( const DataVectorUint8& raw_response, std::size_t payload_length );


        protected:

            const DataVectorUint8 full_message_from_body(const DataVectorUint8& body, uint16_t message_length, std::uint8_t unit_id) const override;
            uint16_t validate_response(const DataVectorUint8& response, const DataVectorUint8& request) const override;
        };

    } // namespace modbus
};   // namespace everest
#endif
