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

////////////////////////////////////////////////////////////////////////////////
//
// test serial connection helper stuff

#include <connection/serial_connection_helper.hpp>

using namespace everest::connection::serial_connection_helper;

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
    // test data stolen from:
    // bsmtool --trace --device /dev/ttyUSB0 get common
   const unsigned char request_common_model[] { 0x2A,0x03,0x9C,0x44,0x00,0x42,0xAD,0xA5 };

   // writeToDevice goes to connection object?
   auto bytes_written = writeToDevice( serial_port, request_common_model, sizeof( request_common_model ));

   ASSERT_EQ( bytes_written , sizeof( request_common_model ) ); // the common model has this size on this device...
   // std::cout << "bytes written: " << bytes_written << std::endl;

   unsigned char readbuffer[ std::numeric_limits<std::uint16_t>::max() ];

   // readFromDevice goes to connection object?
   ::size_t bytes_read = readFromDevice( serial_port, readbuffer, sizeof( readbuffer ) , &tty_config );

   // std::cout << "bytes read: " << bytes_read << "\n";
   EXPECT_EQ( bytes_read , 137 ); // the common model has this size on this device...
   ASSERT_GT( bytes_read, 0 );    // does not make sense to continue testing if we dont read anything...

   // for ( ::size_t index = 0; index < bytes_read; index++ )
   //     std::cout << "index : " << std::dec << std::setw( 4 ) << index << " value: " << std::hex <<  std::setw( 4 ) << (int)readbuffer[index] << "\n";

   std::cout.flush();
   ::close( serial_port );

}

TEST(RTUTests, test_lowlevel_serial_error ) {

    unsigned char buffer[ 42 ] {};
    termios tty {};

    // test on invalid fd
    ASSERT_THROW( writeToDevice( -1, buffer, sizeof( buffer ) ) , std::runtime_error );
    ASSERT_THROW( readFromDevice( -1, buffer, sizeof( buffer ) , &tty ), std::runtime_error );

}
