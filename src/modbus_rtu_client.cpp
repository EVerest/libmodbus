// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>
#include <string>
#include <consts.hpp>

using namespace everest::modbus;

ModbusRTUClient::ModbusRTUClient(connection::Connection& conn_) : ModbusClient(conn_) {}

ModbusRTUClient::~ModbusRTUClient() {}

// ModbusRTUClient::~ModbusRTUClient() {
//     conn.close_connection();
// }
//     // A Modbus "frame" consists of an Application Data Unit (ADU), which encapsulates a Protocol Data Unit (PDU):[8]

//     // ADU = Address + PDU + Error check.
//     // PDU = Function code + Data.

//     // Modbus RTU frame format

//     // This format is primarily used on asynchronous serial data lines like RS-485/EIA-485. Its name refers to a remote terminal unit.
//     // Name     Length (bits)   Function
//     // Start    3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition)
//     // Address      8   Station address
//     // Function     8   Indicates the function code e.g. "read coils"
//     // Data     n × 8   Data + length will be filled depending on the message type
//     // CRC      16      Cyclic redundancy check
//     // End      3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition) between frames

//     // According to this protocol's specification, a full Modbus frame (ADU) can have a PDU with a maximum size of 253 bytes.

//     // TODO: Seems that the connection object has to implement the framing...?

//     // RTU Framing
//     // In RTU mode, messages start with a silent interval of at least 3.5 character times.
//     // This is most easily implemented as a multiple of character times at the baud rate
//     // that is being used on the network (shown as T1–T2–T3–T4 in the figure below).
//     // The first field then transmitted is the device address.
//     // The allowable characters transmitted for all fields are hexadecimal 0–9, A–F.
//     // Networked devices monitor the network bus continuously, including during the
//     // ‘silent’ intervals. When the first field (the address field) is received, each device
//     // decodes it to find out if it is the addressed device.
//     // Following the last transmitted character, a similar interval of at least 3.5 character
//     // times marks the end of the message. A new message can begin after this interval.
//     // The entire message frame must be transmitted as a continuous stream. If a silent
//     // interval of more than 1.5 character times occurs before completion of the frame,
//     // the receiving device flushes the incomplete message and assumes that the next
//     // byte will be the address field of a new message.
//     // Similarly, if a new message begins earlier than 3.5 character times following a
//     // previous message, the receiving device will consider it a continuation of the
//     // previous message. This will set an error, as the value in the final CRC field will not
//     // be valid for the combined messages.


#include <iostream>
#include <iomanip>
#include <limits>

ModbusRTUClient::DataVector ModbusRTUClient::response_without_protocol_data( ModbusRTUClient::DataVector raw_response, std::size_t payload_length ) {
    // strip address and function bytes, but includes bytecount byte
    // TODO: does it make sense to include the byte number?
    return ModbusRTUClient::DataVector( raw_response.cbegin() + 2 , raw_response.cbegin() + 2 + payload_length + 1 );

}

const ModbusRTUClient::DataVector ModbusRTUClient::read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes ) const {

    const std::size_t ModbusRTUQueryLength { 8 };

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    union {
        uint16_t value_16;
        struct {
            unsigned char low;
            unsigned char hi;
        } value_8;
    } swapit;

    DataVector outgoing_data( ModbusRTUQueryLength ); // capacity and size == ModbusRTUQueryLength

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

    modbus::ModbusRTUClient::DataVector response = conn.receive_bytes( everest::modbus::consts::MAX_MESSAGE_SIZE );
    // TODO: check the number of bytes that can be read here
    // Verify: max modbus payload size unit8_t --> 256
    // + function code
    // + unit id
    // + crc16

    // Frame description
    // Slave Address Function Code Data CRC
    // 1byte 1 byte 0 up to 252 byte(s) 2 bytes CRC Low CRCHi
    // The maximum size of a MODBUS RTU frame is 256 byte    auto response = conn.receive_bytes( std::numeric_limits<std::uint16_t>::max());

    // Finally, the ADU includes a PDU. The length of this PDU is still limited
    // to 253 bytes for the standard protocol. The RTU ADU appears to be much
    // simpler

    // Unlike the more complex TCP/IP ADU, this ADU includes only two pieces of
    // information in addition to the core PDU. First, an address is used to
    // define which slave a PDU is intended for. On most networks, an address of
    // 0 defines the “broadcast” address. That is, a master may send an output
    // command to address 0 and all slaves should process the request but no
    // slave should respond. Besides this address, a CRC is used to ensure the
    // integrity of the data.



       // Finally, the ADU includes a PDU. The length of this PDU is still limited to 253 bytes for the standard protocol.

    // returns the payload size, so that we can return a DataVector that does not contain protocol data.
    uint16_t payload_size = validate_response(response, outgoing_data ) ;

    return return_only_registers_bytes ? response_without_protocol_data( response , payload_size ) : response;

    #else

    static_assert( false , "implementaion done only for little endian" );

    #endif

}

const everest::modbus::ModbusRTUClient::DataVector ModbusRTUClient::full_message_from_body(const DataVector& body, uint16_t message_length, MessageDataType unit_id) const {

    using namespace std::string_literals;

    throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " not implemented." );

}

uint16_t everest::modbus::ModbusRTUClient::validate_response(const DataVector& response, const DataVector& request) const {

    using namespace std::string_literals;

    if ( response.size() > ::everest::modbus::consts::MAX_MESSAGE_SIZE )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " response size " + std::to_string( response.size() ) + " is larger than max allowed message size " + std::to_string( ::everest::modbus::consts::MAX_MESSAGE_SIZE ) + " !");

    if ( response.at( 0 ) != request.at( 0 ) )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " request / response unit id mismatch. " );

    if ( response.at( 1 ) != request.at( 1 ) )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " request / response function id mismatch. " );

    uint16_t result_size = response.at( 2 );


    return result_size;

}
