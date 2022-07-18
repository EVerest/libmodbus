#include <gtest/gtest.h>
// #include <gmock/gmock.h>

#include <modbus/utils.hpp>

using namespace everest::modbus::utils;

TEST(RTUTests, test_crc16 ) {

    // check crc16 implementation used for modbus rtu

    {
        // example from https://en.wikipedia.org/wiki/Modbus#Modbus_RTU_frame_format
        const PayloadType payload[] = { 0x01, 0x04, 0x02, 0xFF, 0xFF };
        const CRCResultType expectedCRC{ 0xb880 };
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload)/sizeof(PayloadType));
        ASSERT_EQ( crcResult, expectedCRC);
    }

    // the following sample data generated with https://github.com/chargeITmobility/bsm-python
    {
        // bsmtool --trace --device /dev/ttyUSB0 get ac_meter
        // out: 2A 03 9C 9C 00 69 6D 81
        // request
        const PayloadType payload[] { 0x2A,0x03,0x9C,0x9C,0x00,0x69 };
        const CRCResultType expectedCRC = 0x6d81;
        CRCResultType crcResult = calcCRC_16_ANSI( payload, sizeof(payload)/sizeof(PayloadType) );
        ASSERT_EQ( crcResult, expectedCRC);

    }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get ac_meter
        // response
        const PayloadType payload[] = { 0x2A,0x03,0xD2,0x00,0x13,0x00,0x13,0x00,0x00,0x00,0x00,0xFF,0xFE,0x80,0x00,0x08,0xEF,0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xFF,0xFF,0x01,0xF4,0xFF,0xFF,0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFE,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x03,0x07,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x6A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00 };
        const CRCResultType expectedCRC = 0xE9A2;
        CRCResultType crcResult = calcCRC_16_ANSI( payload, sizeof(payload)/sizeof(PayloadType) );
        ASSERT_EQ( crcResult, expectedCRC);
    }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get common
        // request
        // /dev/ttyUSB0:42[addr=40004] ->2A039C440042ADA5

        const PayloadType payload[] {0x2A,0x03,0x9C,0x44,0x00,0x42 };
        const CRCResultType expectedCRC = 0xada5;
        CRCResultType crcResult = calcCRC_16_ANSI( payload, sizeof(payload)/sizeof(PayloadType) );
        ASSERT_EQ( crcResult, expectedCRC);
     }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get common
        // response
        // /dev/ttyUSB0:42[addr=40004] <--2A0384424155455220456C656374726F6E69630000000000000000000000000000000042534D2D57533336412D4830312D313331312D3030303000000000000000000000000000000000000000000000000000312E393A333243413A414646340000003231303730303139000000000000000000000000000000000000000000000000002A8000C856

        const PayloadType payload[] { 0x2A,0x03,0x84,0x42,0x41,0x55,0x45,0x52,0x20,0x45,0x6C,0x65,0x63,0x74,0x72,0x6F,0x6E,0x69,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x53,0x4D,0x2D,0x57,0x53,0x33,0x36,0x41,0x2D,0x48,0x30,0x31,0x2D,0x31,0x33,0x31,0x31,0x2D,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x2E,0x39,0x3A,0x33,0x32,0x43,0x41,0x3A,0x41,0x46,0x46,0x34,0x00,0x00,0x00,0x32,0x31,0x30,0x37,0x30,0x30,0x31,0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A,0x80,0x00 };
        const CRCResultType expectedCRC = 0xc856;
        CRCResultType crcResult = calcCRC_16_ANSI( payload, sizeof(payload)/sizeof(PayloadType) );
        ASSERT_EQ( crcResult, expectedCRC);

    }
}

#include <stdio.h>
#include <string.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
                    //

#include <stdexcept>
#include <string>

int openSerialDevice( const std::string device ) {

    // using namespace std::string_literals;

    int fd = open( device.c_str() , O_RDWR );

    if ( not ( fd == -1 ) )
        return fd;

    throw std::runtime_error( "Error open serial device: " + device + " Reason: " + strerror( errno ) );

}

void getConfiguration( int serial_port_fd, termios* tty ) {

    if( tcgetattr(serial_port_fd, tty) != 0 ) {
        int myerror = errno;
        throw std::runtime_error( "Error " + std::to_string( myerror ) + " from tcgetattr: " + strerror(myerror));
    }

}

void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ ) {

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

void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds ) {
   // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
   // play with this... we will see if this is needed.
   // VMIN is a character count ranging from 0 to 255 characters, and VTIME is time measured in 0.1 second intervals, (0 to 25.5 seconds).
   tty->c_cc[VTIME] = timeout_deciseconds; // wait at most for timeout_deciseconds for the first char to read
   tty->c_cc[VMIN]  = 0;
}

void configureDevice( int serial_port_fd, termios* tty ) {

   // Save tty settings, also checking for error
   if ( tcsetattr(serial_port_fd, TCSANOW, tty) != 0) {
       int myerror = errno;
       throw std::runtime_error("Error " + std::to_string( myerror ) + " from tcsetattr: " + strerror( myerror ));
   }
}

template<typename T>
std::size_t size_in_byte(const T& array) {
    return sizeof(array) / sizeof(array[0] );
}

TEST(RTUTests, test_lowlevel_serial ) {

    ASSERT_THROW( openSerialDevice("/dev/does_not_exist" ), std::runtime_error );

    // PLAN: this will be moved into a method in the connection object
    int serial_port = openSerialDevice("/dev/ttyUSB0" ); // throws std::runtime_error if open fails
    termios tty_config;
    getConfiguration( serial_port, &tty_config );
    updateConfiguration( &tty_config /*, options */ );
    updateTimeoutConfiguration( &tty_config, 50 );
    configureDevice( serial_port, &tty_config );

    // Test starts here.
   const unsigned char request_common_model[] { 0x2A,0x03,0x9C,0x44,0x00,0x42,0xAD,0xA5 };

   int bytes_written = write( serial_port, request_common_model , sizeof(request_common_model) );

   std::cout << "bytes written: " << bytes_written << std::endl;

   unsigned char readbuffer[ std::numeric_limits<std::uint16_t>::max() ];

   bool changedTiming = false;

   for( int x = 0 ; x < 100 ; ++x ) {
       std::size_t n = read( serial_port, &readbuffer, sizeof( readbuffer ) );

       if ( not changedTiming ) {
           updateTimeoutConfiguration( &tty_config, 2 );
           configureDevice( serial_port, &tty_config );
           changedTiming = true;
      }

       std::cout << std::dec << "bytes read: " << n << std::endl;

       for( std::size_t i = 0; i < n ; ++i )
           std::cout << std::dec << "byte number : " << i << " char value: " << std::hex << readbuffer[ i ] << " hex value: 0x" << std::hex << (int)readbuffer[ i ] << "\n";

       if ( n == 0 )
           break;
   }
   std::cout.flush();
}
