// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>
#include <string>
#include <consts.hpp>
#include <sstream>
// #include <iostream>
#include <iomanip>
#include <limits>


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


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

union SwapIt {
    uint16_t value_16;
    struct {
        unsigned char low;
        unsigned char hi;
    } value_8;
} ;

#endif


DataVectorUint8 ModbusRTUClient::response_without_protocol_data( const DataVectorUint8& raw_response, std::size_t payload_length ) {
    // strip address and function bytes, but includes bytecount byte
    // TODO: does it make sense to include the byte number?
    return DataVectorUint8( raw_response.cbegin() + 2 , raw_response.cbegin() + 2 + payload_length + 1 );

}

const DataVectorUint8 ModbusRTUClient::read_holding_register(uint8_t unit_id, uint16_t first_register_address, uint16_t num_registers_to_read, bool return_only_registers_bytes ) const {

    const std::size_t ModbusRTUQueryLength { 8 };

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    SwapIt swapit;

    DataVectorUint8 outgoing_data( ModbusRTUQueryLength ); // capacity and size == ModbusRTUQueryLength

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

    modbus::DataVectorUint8 response = conn.receive_bytes( max_adu_size() );

    uint16_t payload_size = validate_response(response, outgoing_data ) ;

    return return_only_registers_bytes ? response_without_protocol_data( response , payload_size ) : response;

    #else

    static_assert( false , "implementaion done only for little endian" );

    #endif

}

DataVectorUint8 ModbusDataContainerUint16::get_payload_as_bigendian() const {

    DataVectorUint8 result;
    result.reserve( m_payload.size() * sizeof( DataVectorUint16::value_type ));

    // if ( m_byte_order == ByteOrder::BigEndian )
    if ( m_byte_order == ByteOrder::LittleEndian )
        for( uint16_t value : m_payload ) {
            SwapIt s { value };
            result.push_back( s.value_8.hi );
            result.push_back( s.value_8.low  );
        }
    else
        for( uint16_t value : m_payload ) {
            SwapIt s { value };
            result.push_back( s.value_8.low );
            result.push_back( s.value_8.hi  );
        }

    return result;
}

DataVectorUint8 everest::modbus::ModbusRTUClient::writer_multiple_registers( uint8_t unit_id,
                                                                  uint16_t first_register_address,
                                                                  uint16_t num_registers_to_write,
                                                                  const ModbusDataContainerUint16& payload,
                                                                  bool return_only_registers_bytes
    ) {

    // TODO: write test for this!!!:

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    using namespace std::string_literals;

    if ( num_registers_to_write > 123 )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " only max 123 register allowed to be written, request was: " + std::to_string(num_registers_to_write) + " register" );

    DataVectorUint8 outgoing_payload = payload.get_payload_as_bigendian();

    std::size_t outgoing_data_size {
        1 + // unit id
        1 + // function code
        2 + // starting address
        2 + // quantity of registers
        1 + // byte count
        outgoing_payload.size() +
        2   // crc16
    };

    DataVectorUint8 outgoing_data( outgoing_data_size );
    SwapIt swapit;
    outgoing_data[ 0 ] = unit_id;
    outgoing_data[ 1 ] = 0x10; // function code "write multiple registers".

    swapit.value_16 = first_register_address ;
    outgoing_data[ 2 ] = swapit.value_8.hi;
    outgoing_data[ 3 ] = swapit.value_8.low;

    swapit.value_16 = num_registers_to_write ;
    outgoing_data[ 4 ] = swapit.value_8.hi;
    outgoing_data[ 5 ] = swapit.value_8.low;

    outgoing_data[ 6 ] = num_registers_to_write * 2; // 16 bit register for now

    auto it = outgoing_data.begin() + 7;
    for ( auto data: outgoing_payload )
        (*it++) = data;

    swapit.value_16 = everest::modbus::utils::calcCRC_16_ANSI(outgoing_data.data(), outgoing_data_size - 2 ); // crc for the whole container, omitting the last two unit8 for the crc itself.

    (*(outgoing_data.end() - 2)) = swapit.value_8.hi;
    (*(outgoing_data.end() - 1)) = swapit.value_8.low;

    conn.send_bytes( outgoing_data );

    modbus::DataVectorUint8 response = conn.receive_bytes( max_adu_size() );

    uint16_t payload_size = validate_response(response, outgoing_data ) ;

    return return_only_registers_bytes ? response_without_protocol_data( response , payload_size ) : response;

    #else

    static_assert( false , "implementaion done only for little endian" );

    #endif

}


const everest::modbus::DataVectorUint8 ModbusRTUClient::full_message_from_body(const DataVectorUint8& body, uint16_t message_length, std::uint8_t unit_id) const {

    using namespace std::string_literals;

    throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " not implemented." );

}

uint16_t everest::modbus::ModbusRTUClient::validate_response(const DataVectorUint8& response, const DataVectorUint8& request) const {

    using namespace std::string_literals;

    if ( response.size() > max_adu_size() )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " response size " + std::to_string( response.size() ) + " is larger than max allowed message size " + std::to_string( max_adu_size() ) + " !");

    // FIXME: What happens in case the request was a broadcast?
    if ( response.at( 0 ) != request.at( 0 ) )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " request / response unit id mismatch. " );

    if ( not ( (response.at( 1 ) & 0x80) == 0 ) ) {
        uint8_t exception_code = response.at( 2 );
        std::stringstream ss;
        std::string error_message;
        switch ( exception_code ) {
        case 0x01:
            error_message = "ILLEGAL FUNCTION";
            break;
        case 0x02:
            error_message = "ILLEGAL DATA ADDRESS";
            break;
        case 0x03:
            error_message = "ILLEGAL DATA VALUE";
            break;
        case 0x04:
            error_message = "SERVER DEVICE FAILURE";
            break;
        case 0x05:
            error_message = "ACKNOWLEDGE";
            break;
        case 0x06:
            error_message = "SERVER DEVICE BUSY";
            break;
        // case 0x07: does not exist
        case 0x08:
            error_message = "MEMORY PARITY ERROR";
            break;
        // case 0x09: does not exist
        case 0x0a:
            error_message = "GATEWAY PATH UNAVAILABLE";
            break;
        case 0x0b:
            error_message = "GATEWAY TARGET DEVICE FAILED TO RESPOND";
            break;
        default:
            error_message = "UNKNOWN ERROR";
        }
        ss << __PRETTY_FUNCTION__ << "  response returned an error code: " << std::hex << (int)exception_code << " ( " << error_message << " ) ";
        throw std::runtime_error( ss.str() );
    }

    if ( response.at( 1 ) != request.at( 1 ) )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " request / response function id mismatch. " );

    uint16_t result_size = response.at( 2 );

    return result_size;

}
