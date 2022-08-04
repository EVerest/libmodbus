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

#include <termios.h>
#include <fcntl.h> // Contains file controls like O_RDWR

class SerialDevice {

public:

    termios tty_config {};
    int     fd = -1 ;

    virtual int openSerialDevice( const std::string device );
    virtual int closeSerialDevice( int serial_port_fd );
    virtual void getConfiguration( int serial_port_fd, termios* tty );
    virtual void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ );
    virtual void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds );
    virtual void configureDevice( int serial_port_fd, termios* tty );
    virtual ::size_t writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count );
    virtual ::size_t readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config );

};

namespace serial_connection_helper {

    int openSerialDevice( const std::string device );
    int closeSerialDevice( int serial_port_fd );
    void getConfiguration( int serial_port_fd, termios* tty );
    void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ );
    void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds );
    void configureDevice( int serial_port_fd, termios* tty );
    ::size_t writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count );
    ::size_t readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config );

}

int SerialDevice::openSerialDevice( const std::string device ){

    return ::serial_connection_helper::openSerialDevice( device );

}

int SerialDevice::closeSerialDevice( int serial_port_fd ){

    return ::serial_connection_helper::closeSerialDevice( serial_port_fd );

}

void SerialDevice::getConfiguration( int serial_port_fd, termios* tty ) {

    return ::serial_connection_helper::getConfiguration( serial_port_fd, tty );

}

void SerialDevice::updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ ){

    ::serial_connection_helper::updateConfiguration( tty );

}
void SerialDevice::updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds ){

    ::serial_connection_helper::updateTimeoutConfiguration( tty , timeout_deciseconds );

}
void SerialDevice::configureDevice( int serial_port_fd, termios* tty ){

    ::serial_connection_helper::configureDevice( serial_port_fd, tty );

}
::size_t SerialDevice::writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count ){

    return ::serial_connection_helper::writeToDevice( serial_port_fd, buffer, count );

}

::size_t SerialDevice::readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config ) {

    return ::serial_connection_helper::readFromDevice( serial_port_fd, buffer, count, tty_config );

}




int serial_connection_helper::openSerialDevice( const std::string device ) {

    int fd = open( device.c_str() , O_RDWR );

    if ( not ( fd == -1 ) )
        return fd;

    throw std::runtime_error( "Error open serial device: " + device + " Reason: " + strerror( errno ) );

}

int serial_connection_helper::closeSerialDevice( int serial_port_fd ) {

    return ::close( serial_port_fd );
}

void serial_connection_helper::getConfiguration( int serial_port_fd, termios* tty ) {

    if( tcgetattr(serial_port_fd, tty) != 0 ) {
        int myerror = errno;
        throw std::runtime_error( "Error " + std::to_string( myerror ) + " from tcgetattr: " + strerror(myerror));
    }

}

void serial_connection_helper::updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ ) {

    // using namespace std::string_literals;

    // we are a bit verbose with these settings...
    // BSM power meter used this as default:
    // 19200 Baud
    cfsetspeed( tty, B19200);
    // Parity
    tty->c_cflag  |= PARENB;
    // 8 Data bits
    tty->c_cflag &= ~CSIZE;
    tty->c_cflag |= CS8;
    // 1 Stop bit
    tty->c_cflag &= ~CSTOPB;
    // no CRTSCTS (?)
    tty->c_cflag &= ~CRTSCTS;

    // turn off modem specific stuff...
    tty->c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    // disable canonical mode (in canonical mode, data is processed after newline).
    tty->c_lflag &= ~ICANON;

    tty->c_lflag &= ~ECHO; // Disable echo
    tty->c_lflag &= ~ECHOE; // Disable erasure
    tty->c_lflag &= ~ECHONL; // Disable new-line echo

    tty->c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty->c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty->c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

 }

void serial_connection_helper::updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds ) {
   // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
   // play with this... we will see if this is needed.
   // VMIN is a character count ranging from 0 to 255 characters, and VTIME is time measured in 0.1 second intervals, (0 to 25.5 seconds).
   tty->c_cc[VTIME] = timeout_deciseconds; // wait at most for timeout_deciseconds for the first char to read
   tty->c_cc[VMIN]  = 0;
}

void serial_connection_helper::configureDevice( int serial_port_fd, termios* tty ) {

   // Save tty settings, also checking for error
   if ( tcsetattr(serial_port_fd, TCSANOW, tty) != 0) {
       int myerror = errno;
       throw std::runtime_error("Error " + std::to_string( myerror ) + " from tcsetattr: " + strerror( myerror ));
   }
}

::size_t serial_connection_helper::writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count ) {

    ::size_t bytes_written_sum = 0;

    while ( bytes_written_sum < count ) {
    // write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
    // On success, the number of bytes written is returned.  On error, -1 is returned, and errno is set to indicate the error.
        ssize_t bytes_written = ::write( serial_port_fd, buffer, count );
        if ( bytes_written == -1 ) {
            // handle error
            int myerror = errno;
            throw std::runtime_error( "Error: " + std::to_string( myerror ) + " from ::write: " + strerror( myerror ));
        }
        bytes_written_sum += bytes_written;
    }
    return bytes_written_sum;
}

::ssize_t readByteFromDevice( int fd, unsigned char* charToRead ) {

    ssize_t read_result = ::read( fd, charToRead, 1 );
    if( not ( read_result == -1 ))
        return read_result;

    int myerror = errno;
    throw std::runtime_error("Error: " + std::to_string( myerror ) + " from ::read: " + strerror( myerror ));

}

::size_t serial_connection_helper::readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config ) {

    static_assert( std::is_unsigned<decltype(count)>::value , "need an unsigned type here. ");

    if ( count == 0 )
        return 0;

    // read at least one byte
    std::size_t index = 0;
    ::size_t bytes_read = readByteFromDevice( serial_port_fd, &buffer[ index++ ] );
    if ( bytes_read == 0 )
        return 0;

    // dont wait that long after the end of transmission as it is done at the beginning.
    updateTimeoutConfiguration( tty_config, 2 );
    configureDevice( serial_port_fd, tty_config );

    while ( bytes_read < count ) {

        auto bytes_current = readByteFromDevice( serial_port_fd, &buffer[ index++ ] );
        if ( bytes_current == 0 )
            return bytes_read;
        bytes_read += bytes_current;

    }
    return bytes_read;
}


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

using namespace  std::string_literals;

namespace ModbusMessages {

    struct Data {
        DataVector data;
        DataVector::iterator payload_begin { data.end() };
        DataVector::iterator payload_end { data.end() };
    };

    using DataSpt = std::shared_ptr<Data>;

    class AbstractModbusQuery {

    public:

        virtual bool is_ready() const = 0;
        virtual std::size_t size() const = 0;
        virtual void append_query_to( DataVectorSpt ) const = 0;

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

        void append_query_to( DataVectorSpt outgoing ) const override {

                outgoing -> push_back( high_byte( m_register_address.value() ));
                outgoing -> push_back( low_byte(  m_register_address.value() ));
                outgoing -> push_back( high_byte( m_number_of_register_to_read.value() ));
                outgoing -> push_back( low_byte(  m_number_of_register_to_read.value() ));

        }
    };
    class AbstractModbusResponse {

    protected:

        DataVectorSpt m_data_spt {};

    public:

        std::size_t size() const { return m_data_spt ? m_data_spt -> size() : 0; }
        virtual void set_buffer( const DataVectorSpt data_spt ) {
            m_data_spt = data_spt;
        }
        virtual DataVector response_data() const = 0;

    };

    class ReadHoldingRegisterResponse : public AbstractModbusResponse {

    public:

        virtual DataVector response_data() const {

            if ( not m_data_spt )
                throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " no data buffer available!" );
            return DataVector( );
        }

    };


}

namespace ModbusTransport {

    struct RTUConfig {

        std::string device { "/dev/ttyUSB0" };
        std::uint8_t unit_id;
        // std::string m_baud = "19200"; //hardcoded for now
        unsigned int m_initial_wait_deciseconds = 50; // wait 5 secs for response
    };


    class RTU {

    private:

        RTUConfig m_config {};

        SerialDevice m_serial_device;

        DataVectorSpt m_data_spt;

        int send_buffer() { // throws runtime_error
            return m_serial_device.writeToDevice( m_serial_device.fd, m_data_spt->data(), m_data_spt->size() );
        }

        int read_response() {
            // CONT HERE!
        }

    public:

        RTU( RTUConfig config ) :
            m_config( config )
            {}

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

        void send( const ModbusMessages::AbstractModbusQuery& query ) {

            std::size_t full_query_size = query.size()
                + 1 // unit id
                + 2; // crc

            m_data_spt = std::make_shared<DataVector>();
            // TODO: check max message size

            m_data_spt -> reserve( full_query_size );
            m_data_spt -> push_back( m_config.unit_id );
            query.append_query_to( m_data_spt );

            std::uint16_t crc16 = calcCRC_16_ANSI( m_data_spt->data(), m_data_spt->size() );
            m_data_spt -> push_back( high_byte( crc16 )); // check if this is ok...
            m_data_spt -> push_back( low_byte( crc16 ));

        }

        void operator << ( const ModbusMessages::AbstractModbusQuery& query ) {
            send( query );
        }

        void operator >> ( ModbusMessages::AbstractModbusResponse& response ) {
            response.set_buffer( m_data_spt );
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
