#include "serial_device.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <tuple>
#include <memory>
#include <stdexcept>
#include <string>
#include <iterator>




std::uint16_t calcCRC_16_ANSI( const std::uint8_t* payload, std::size_t payload_length ) {

    // https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Polynomial_representations_of_cyclic_redundancy_checks
    // implementation stolen from: https://modbus.org/docs/PI_MBUS_300.pdf

    // High-Order Byte Table
    /* Table of CRC values for high–order byte */
    const std::uint8_t auchCRCHi[] = {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
        0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
        0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
        0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
        0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
        0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
        0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
        0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
        0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
        0x40
    } ;

    /* Table of CRC values for low–order byte */
    const std::uint8_t auchCRCLo[] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
        0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
        0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
        0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
        0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
        0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
        0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
        0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
        0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
        0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
        0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
        0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
        0x40
    } ;

    std::uint8_t uchCRCHi = 0xff;
    std::uint8_t uchCRCLo = 0xff;

    while ( payload_length-- ) {
        std::size_t uIndex = uchCRCHi ^ *payload++;
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }

    return (uchCRCHi << 8 | uchCRCLo) ;

}


using DataVector = std::vector<std::uint8_t>;
using DataVectorSpt = std::shared_ptr<DataVector>;

inline std::uint8_t high_byte ( std::uint16_t val ) { return ( val >> 8 ) & 0xff ; }
inline std::uint8_t low_byte  ( std::uint16_t val ) { return val & 0xff ; }

inline std::uint16_t big_endian( std::uint8_t high_byte, std::uint8_t low_byte ) { return ( high_byte << 8 ) | low_byte; }

using namespace  std::string_literals;

namespace ModbusMessages {

    struct ResponseData {
        DataVector data;
        DataVector::const_iterator payload_begin { data.begin() };
        DataVector::const_iterator payload_end { data.end() };
    };

    namespace RTU {
        constexpr uint16_t MAX_ADU = 256;
        constexpr uint16_t MAX_PDU = 253;
    }

    using ResponseDataSpt = std::shared_ptr<ResponseData>;

    class AbstractModbusQuery {

    public:

        virtual bool is_ready() const = 0;
        virtual std::size_t size() const = 0;
        virtual void append_query_to( DataVector& ) const = 0;

    };

    class ReadHoldingRegisterQuery : public AbstractModbusQuery {

    private:

        std::optional<std::uint16_t> m_register_address;
        std::optional<std::uint8_t> m_number_of_register_to_read;

    public:

        ReadHoldingRegisterQuery& register_address_unit16( std::uint16_t read_register_address ) {

            m_register_address = read_register_address;
            return *this;
        }

        ReadHoldingRegisterQuery& number_of_register_to_read( std::uint8_t number_of_register_to_read) {

            m_number_of_register_to_read = number_of_register_to_read;
            return *this;
        }

        std::size_t size() const override { return
                1 + // function code
                2 + // register address
                2; } // number of register to read

        bool is_ready() const override { return m_register_address.has_value() and m_number_of_register_to_read.has_value(); }

        void append_query_to( DataVector& outgoing ) const override {

                outgoing.push_back( high_byte( m_register_address.value() ));
                outgoing.push_back( low_byte(  m_register_address.value() ));
                outgoing.push_back( high_byte( m_number_of_register_to_read.value() ));
                outgoing.push_back( low_byte(  m_number_of_register_to_read.value() ));

        }
    };

    class ModbusResponse {

    protected:

        ResponseData m_data {};

    public:

        bool empty() const { return size_raw() == 0; }
        std::size_t size_raw()      const { return m_data.data.size(); }
        std::size_t size_payload () const {
            return
                m_data.payload_begin > m_data.payload_end ?
                0 : // the iterator seem to point somewhere into the nothing...
                size_raw() -
                ( std::distance( m_data.data.begin() , m_data.payload_begin ) +
                  std::distance( m_data.payload_end  , m_data.data.end() ));
        }

        ResponseData& get_response_data_container() { return m_data; }

        // need this?
        DataVector response_data() const {

            DataVector result;
            result.reserve( size_payload() );
            std::copy( m_data.payload_begin, m_data.payload_end, std::back_inserter( result ) );
            return result;

        }

        static inline bool is_exception_code( std::uint8_t code ) { return not ( ( code & 0x80 ) == 0 ); }
        static inline bool function_code_without_exception_flag( std::uint8_t code ) { return code & 0x7f; }

        std::uint8_t function_code() const {

            if ( empty() )
                throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " Attempt to get function code from empty / invalid response.");

            return (*m_data.payload_begin);
        }

    };

    class ReadHoldingRegisterResponse : public ModbusResponse {

    public:

    };


}

namespace ModbusTransport {

    struct RTUConfig {

        std::string device { "/dev/ttyUSB0" };
        std::uint8_t unit_id;
        // std::string m_baud = "19200"; //hardcoded for now
        unsigned int m_initial_wait_deciseconds = 50; // wait 5 secs for response
    };

    class AbstractModbusTransport {

    public:

        // max number of bytes to transfer (another name for adu)
        virtual std::size_t max_message_size() const = 0;

        // max number of bytes to transfer (application data unit), payload and protocol bytes.
        virtual std::size_t max_adu_size()     const = 0;

        // max payload size (function code and function specific data).
        virtual std::size_t max_pdu_size()     const = 0;

        virtual void send( const ModbusMessages::AbstractModbusQuery&  ) = 0;
        virtual void read( ModbusMessages::ModbusResponse&  ) = 0;
        virtual void verify_response( ModbusMessages::ModbusResponse& ) const = 0;

    };

    struct RTUTransaction {
        std::uint8_t m_unit_id;
        std::uint8_t m_function_code;
    };

    class RTU : public AbstractModbusTransport {

    private:

        RTUConfig m_config {};

        SerialDevice m_serial_device;

        RTUTransaction m_transaction;

    public:

        RTU( RTUConfig config ) :
            m_config( config )
            {}

        // max number of bytes to transfer (another name for adu)
        std::size_t max_message_size() const override { return max_adu_size(); }

        // max number of bytes to transfer.
        std::size_t max_adu_size() const override { return ModbusMessages::RTU::MAX_ADU; }

        // max payload size (function code and function specific data).
        std::size_t max_pdu_size() const override { return ModbusMessages::RTU::MAX_PDU; }

        bool connected() const { return not ( m_serial_device.fd == -1 ); }

        void connect() {

            close();
            m_serial_device.fd = m_serial_device.openSerialDevice(m_config.device);

            m_serial_device.getConfiguration( m_serial_device.fd, &m_serial_device.tty_config );
            m_serial_device.updateConfiguration( &m_serial_device.tty_config /*, options */ );
            m_serial_device.updateTimeoutConfiguration( &m_serial_device.tty_config, m_config.m_initial_wait_deciseconds );
            m_serial_device.configureDevice( m_serial_device.fd, &m_serial_device.tty_config );

        }

        void close() {
            m_serial_device.closeSerialDevice(m_serial_device.fd);
        }

        void verify_exception_code( ModbusMessages::ModbusResponse& response ) const {

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

        // rtu implementaion
        void verify_response( ModbusMessages::ModbusResponse& response ) const override {

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

        // this is the rtu transport
        void verify_transaction( ModbusMessages::ModbusResponse& response ) {

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

        void send( const ModbusMessages::AbstractModbusQuery& query ) override {

            std::size_t full_query_size = query.size()
                + 1 // unit
                + 2; // crc

            DataVector buffer;
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

        void read( ModbusMessages::ModbusResponse& response ) override {

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

        void operator << ( const ModbusMessages::AbstractModbusQuery& query ) {
            send( query );
        }

        void operator >> ( ModbusMessages::ModbusResponse& response ) {
            read( response );
        }
    };
}



TEST( RTUClientHighlevel, test_rtu_client_messages ) {

    ModbusMessages::ReadHoldingRegisterQuery read_holding_register;

    read_holding_register
        .register_address_unit16( 40004 )
        .number_of_register_to_read( 66 );

    ASSERT_TRUE( read_holding_register.is_ready() );

    ModbusTransport::RTUConfig config;
    config.unit_id = 42;
    config.device = "/dev/ttyUSB0";

    ModbusTransport::RTU transport(config);
    transport.connect();

    transport << read_holding_register;

    ModbusMessages::ReadHoldingRegisterResponse read_holding_register_response;
    transport >> read_holding_register_response;

}


TEST(TestStl, test_stl) {

    using DataVector = std::vector<std::uint8_t>;

    DataVector payload { 3,4,5,6 };

    DataVector full_container;

    // two at front, function code , payload, two at end
    full_container.reserve( payload.size() + 5 );

    full_container.push_back( 0 );
    full_container.push_back( 1 );

    ASSERT_EQ( full_container.size() , (std::size_t)2 );

    full_container.push_back( 0x2 ); // function code...

    std::copy( payload.cbegin(), payload.cend(), std::back_inserter( full_container ) );
    full_container.push_back( 7 );
    full_container.push_back( 8 );

    ASSERT_EQ( full_container.size() , full_container.capacity() );

    for( auto val : full_container )
        std::cout << std::to_string( val ) << "\n";

    const std::uint16_t val { 0xff00 };

    ASSERT_EQ( high_byte( val ) , 255 );
    ASSERT_EQ( low_byte ( val ) ,   0 );

}

TEST(TestModbusMessages, test_ModbusResponse) {

    ModbusMessages::ModbusResponse res;

    res.get_response_data_container().data = { 0,1,2,3,4,5,6 };

    // verify setup
    ASSERT_EQ( res.size_raw() , (unsigned)7 );
    ASSERT_EQ( res.size_payload() , (unsigned)0 );
    ASSERT_TRUE( res.get_response_data_container().payload_begin ==  res.get_response_data_container().payload_end );

    // fill in information about stuff we dont need / protocol data.
    res.get_response_data_container().payload_begin = res.get_response_data_container().data.cbegin() + 2 ; // strip function code and unit id, here 0,1

    // compare iterator
    ASSERT_TRUE( res.get_response_data_container().payload_begin >  res.get_response_data_container().payload_end );

    // begin points past end, so we dont have any payload, payload size is 0.
    ASSERT_EQ( res.size_payload() , (unsigned)0 );

    res.get_response_data_container().payload_end = res.get_response_data_container().data.cend() - 2; // strip crc from end, here 5,6

    // verify expected payload size.
    ASSERT_EQ( res.size_payload() , (unsigned)3 );

    // compare iterator
    ASSERT_TRUE( res.get_response_data_container().payload_begin <  res.get_response_data_container().payload_end );

    // test that our payload is ok.
    DataVector expected_payload { 2,3,4 };
    ASSERT_EQ( expected_payload , res.response_data() );

}
