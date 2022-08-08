#include "modbus_transport.hpp"

#include <sstream>

#include "crc16.hpp"
#include "endian_tools.hpp"

void ModbusTransport::RTU::connect() {

    close();
    m_serial_device.fd = m_serial_device.openSerialDevice(m_config.device);

    m_serial_device.getConfiguration( m_serial_device.fd, &m_serial_device.tty_config );
    m_serial_device.updateConfiguration( &m_serial_device.tty_config /*, options */ );
    m_serial_device.updateTimeoutConfiguration( &m_serial_device.tty_config, m_config.m_initial_wait_deciseconds );
    m_serial_device.configureDevice( m_serial_device.fd, &m_serial_device.tty_config );

}


void ModbusTransport::RTU::close() {

    m_serial_device.closeSerialDevice(m_serial_device.fd);

}


void ModbusTransport::RTU::verify_exception_code( ModbusMessages::ModbusResponse& response ) const {

    std::uint8_t function_code = response.function_code();

    if ( (  function_code & 0x80 ) == 0 )
        return; // no exception, just a normal function code

    std::stringstream ss;
    std::string error_message;
    switch ( function_code ) {
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

    ss << __PRETTY_FUNCTION__ << "  response returned an error code: " << std::hex << (int)function_code << " ( " << error_message << " ) ";

    throw std::runtime_error( ss.str() );

}


void ModbusTransport::RTU::verify_response( ModbusMessages::ModbusResponse& response ) const {

    using namespace std::string_literals;

    if ( response.size_raw() > max_adu_size() )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " response size " + std::to_string( response.size_raw()  ) + " is larger than max allowed message size " + std::to_string( max_adu_size() ) + " !");

    if ( response.empty() )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " response is empty, maybe timeout on reading device. ");

    verify_exception_code( response );

    std::uint16_t crcResponseCalculated = calcCRC_16_ANSI( response.get_response_data_container().data.data() , response.size_raw() - 2 );
    std::uint16_t crcResponseData = big_endian( (*response.get_response_data_container().data.cend() - 2 ),(*response.get_response_data_container().data.cend() - 1 ) );

    if ( crcResponseCalculated != crcResponseData )
        throw std::runtime_error ( ""s + __PRETTY_FUNCTION__ + " checksum error " );

}


void ModbusTransport::RTU::verify_transaction( ModbusMessages::ModbusResponse& response ) {

    using namespace std::string_literals;

    std::uint8_t function_code = ModbusMessages::ModbusResponse::function_code_without_exception_flag( response.function_code() );

    std::uint8_t unit_id = response.get_response_data_container().data.at( 0 );

    if ( m_transaction.m_unit_id != unit_id )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " transaction error. transaction unit id " + std::to_string( m_transaction.m_unit_id ) +
                                  " not equal to response unit id: " + std::to_string( unit_id ) + " !" );

    if ( m_transaction.m_function_code != function_code )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " transaction error. transaction function code " + std::to_string( m_transaction.m_function_code ) +
                                  " not equal to response function code : " + std::to_string( function_code ) + " !" );

}


void ModbusTransport::RTU::send( const ModbusMessages::AbstractModbusQuery& query )  {

    std::size_t full_query_size = query.size()
        + 1 // unit
        + 2; // crc

    ModbusMessages::DataVector buffer;
    buffer.reserve( full_query_size );
    buffer.push_back( m_config.unit_id );
    query.append_query_to( buffer );

    std::uint16_t crc16 = calcCRC_16_ANSI( buffer.data(), buffer.size() );
    buffer.push_back( high_byte( crc16 )); // check if this is ok...
    buffer.push_back( low_byte( crc16 ));

    m_serial_device.writeToDevice( m_serial_device.fd, buffer.data(), buffer.size() );

    m_transaction.m_function_code = buffer.at( 1 );
    m_transaction.m_unit_id = buffer.at( 0 );

}

void ModbusTransport::RTU::read( ModbusMessages::ModbusResponse& response ) {

    ModbusMessages::ResponseData& response_data{ response.get_response_data_container() };

    response_data.data.reserve( max_message_size() );
    ssize_t bytes_read = m_serial_device.readFromDevice( m_serial_device.fd, response_data.data.data(), max_message_size() , &m_serial_device.tty_config );
    response_data.data.resize( bytes_read );

    // check that the response is formally correct..
    verify_response( response );
    response_data.payload_begin = response_data.data.begin() + 2; // strip unit id and function code
    response_data.payload_end   = response_data.data.end()   - 2;   // strip crc16 at end
    // check that the response is the one we excpected..
    verify_transaction( response );

}


void ModbusTransport::RTU::operator << ( const ModbusMessages::AbstractModbusQuery& query ) {
    send( query );
}


void ModbusTransport::RTU::operator >> ( ModbusMessages::ModbusResponse& response ) {
    read( response );
}
