// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

using namespace everest::modbus;

ModbusRTUClient::ModbusRTUClient(connection::Connection& conn_) : ModbusClient(conn_) {}

ModbusRTUClient::~ModbusRTUClient() {}

// why the hell is conn private in this context??
// ModbusRTUClient::~ModbusRTUClient() {
//     conn.close_connection();
// }

#include <iostream>
#include <iomanip>

const ModbusRTUClient::DataVector ModbusRTUClient::read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes ) const {
    std::cout << __PRETTY_FUNCTION__ << " start " << std::endl;

    const std::size_t ModbusRTUQueryLength { 8 };
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    union {
        uint16_t value_16;
        struct {
            unsigned char low;
            unsigned char hi;
        } value_8;
    }swapit;

    DataVector outgoing_data;
    outgoing_data.reserve( ModbusRTUQueryLength );
    outgoing_data.resize( ModbusRTUQueryLength );

    outgoing_data[ 0 ] = unit_id;
    outgoing_data[ 1 ] = 0x03; // read holding register command

    // register address
    swapit.value_16 = first_register_address;
    outgoing_data[ 2 ] = swapit.value_8.hi;
    outgoing_data[ 3 ] = swapit.value_8.low;

    // number of regs to read
    swapit.value_16 = num_registers_to_read;
    outgoing_data[ 4 ] = swapit.value_8.hi;
    outgoing_data[ 5 ] = swapit.value_8.low;

    // crc16 of payload
    swapit.value_16 = everest::modbus::utils::calcCRC_16_ANSI(outgoing_data.data(), ModbusRTUQueryLength - 2 );
    outgoing_data[ 6 ] = swapit.value_8.hi;
    outgoing_data[ 7 ] = swapit.value_8.low;

    conn.send_bytes(outgoing_data);
    #else

    static_assert( false , "implementaion done only for little endian" );

    #endif

    std::cout << __PRETTY_FUNCTION__ << " end " << std::endl;
    return outgoing_data;

}

const ModbusRTUClient::DataVector ModbusRTUClient::full_message_from_body(const DataVector& body, uint16_t message_length, MessageDataType unit_id) const {

    std::cout << __PRETTY_FUNCTION__ << " start " << std::endl;

    std::cout << "body size: " << body.size() << std::endl;
    for (int i = 0; i < body.size() ; i++) {
        std::cout << "index : " << std::dec << i << " value hex : " << std::hex << std::setw( 4 ) << (int) body.at( i ) << " value dec " << std::dec << std::setw( 4 ) << (int) body.at( i ) << "\n";
    }
    // TBD

    // A Modbus "frame" consists of an Application Data Unit (ADU), which encapsulates a Protocol Data Unit (PDU):[8]

    // ADU = Address + PDU + Error check.
    // PDU = Function code + Data.

    // Modbus RTU frame format

    // This format is primarily used on asynchronous serial data lines like RS-485/EIA-485. Its name refers to a remote terminal unit.
    // Name     Length (bits)   Function
    // Start    3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition)
    // Address      8   Station address
    // Function     8   Indicates the function code e.g. "read coils"
    // Data     n × 8   Data + length will be filled depending on the message type
    // CRC      16      Cyclic redundancy check
    // End      3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition) between frames

    // According to this protocol's specification, a full Modbus frame (ADU) can have a PDU with a maximum size of 253 bytes.

    // TODO: Seems that the connection object has to implement the framing...?

    // RTU Framing
    // In RTU mode, messages start with a silent interval of at least 3.5 character times.
    // This is most easily implemented as a multiple of character times at the baud rate
    // that is being used on the network (shown as T1–T2–T3–T4 in the figure below).
    // The first field then transmitted is the device address.
    // The allowable characters transmitted for all fields are hexadecimal 0–9, A–F.
    // Networked devices monitor the network bus continuously, including during the
    // ‘silent’ intervals. When the first field (the address field) is received, each device
    // decodes it to find out if it is the addressed device.
    // Following the last transmitted character, a similar interval of at least 3.5 character
    // times marks the end of the message. A new message can begin after this interval.
    // The entire message frame must be transmitted as a continuous stream. If a silent
    // interval of more than 1.5 character times occurs before completion of the frame,
    // the receiving device flushes the incomplete message and assumes that the next
    // byte will be the address field of a new message.
    // Similarly, if a new message begins earlier than 3.5 character times following a
    // previous message, the receiving device will consider it a continuation of the
    // previous message. This will set an error, as the value in the final CRC field will not
    // be valid for the combined messages.

    std::cout << __PRETTY_FUNCTION__ << " end " << std::endl;
    return body;
}

std::uint16_t ModbusRTUClient::validate_response(const DataVector& response, const DataVector& request) const {

    return 42;
}
