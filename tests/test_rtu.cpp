// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <connection/connection.hpp>
#include <connection/serial_connection_helper.hpp>
#include <consts.hpp>
#include <modbus/exceptions.hpp>
#include <modbus/utils.hpp>

#include <algorithm>

using namespace everest::modbus::utils;

TEST(RTUTests, test_crc16) {

    // check crc16 implementation used for modbus rtu

    {
        // example from https://en.wikipedia.org/wiki/Modbus#Modbus_RTU_frame_format
        const PayloadType payload[] = {0x01, 0x04, 0x02, 0xFF, 0xFF};
        const CRCResultType expectedCRC{0xb880};
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload) / sizeof(PayloadType));
        ASSERT_EQ(crcResult, expectedCRC);
    }

    // the following sample data generated with https://github.com/chargeITmobility/bsm-python
    {
        // bsmtool --trace --device /dev/ttyUSB0 get ac_meter
        // out: 2A 03 9C 9C 00 69 6D 81
        // request
        const PayloadType payload[]{0x2A, 0x03, 0x9C, 0x9C, 0x00, 0x69};
        const CRCResultType expectedCRC = 0x6d81;
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload) / sizeof(PayloadType));
        ASSERT_EQ(crcResult, expectedCRC);
    }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get ac_meter
        // response
        const PayloadType payload[] = {
            0x2A, 0x03, 0xD2, 0x00, 0x13, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x80, 0x00, 0x08, 0xEF, 0x00,
            0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xFF, 0xFF, 0x01, 0xF4, 0xFF, 0xFF, 0x00,
            0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0xFF, 0xFE, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x03, 0x07, 0x00, 0x00, 0x00,
            0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x09, 0x6A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
        const CRCResultType expectedCRC = 0xE9A2;
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload) / sizeof(PayloadType));
        ASSERT_EQ(crcResult, expectedCRC);
    }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get common
        // request
        // /dev/ttyUSB0:42[addr=40004] ->2A039C440042ADA5

        const PayloadType payload[]{0x2A, 0x03, 0x9C, 0x44, 0x00, 0x42};
        const CRCResultType expectedCRC = 0xada5;
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload) / sizeof(PayloadType));
        ASSERT_EQ(crcResult, expectedCRC);
    }

    {
        // bsmtool --trace --device /dev/ttyUSB0 get common
        // response
        // /dev/ttyUSB0:42[addr=40004]
        // <--2A0384424155455220456C656374726F6E69630000000000000000000000000000000042534D2D57533336412D4830312D313331312D3030303000000000000000000000000000000000000000000000000000312E393A333243413A414646340000003231303730303139000000000000000000000000000000000000000000000000002A8000C856

        const PayloadType payload[]{
            0x2A, 0x03, 0x84, 0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E,
            0x69, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x42, 0x53, 0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33,
            0x31, 0x31, 0x2D, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E,
            0x39, 0x3A, 0x33, 0x32, 0x43, 0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30,
            0x37, 0x30, 0x30, 0x31, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00};
        const CRCResultType expectedCRC = 0xc856;
        CRCResultType crcResult = calcCRC_16_ANSI(payload, sizeof(payload) / sizeof(PayloadType));
        ASSERT_EQ(crcResult, expectedCRC);
    }
}

TEST(RTUTests, test_ModbusDataContainerUint16) {

    using namespace everest::modbus;

    ModbusDataContainerUint16 big_endian(ByteOrder::BigEndian, {0x00ff, 0x2442});
    ModbusDataContainerUint16 little_endian(ByteOrder::LittleEndian, {0xff00, 0x4224});

    {
        DataVectorUint8 res = big_endian.get_payload_as_bigendian();
        ASSERT_EQ(res.size(), big_endian.size() * 2); // number of elements in container
        ASSERT_EQ(res[0], 0xff);
        ASSERT_EQ(res[1], 0x00);
        ASSERT_EQ(res[2], 0x42);
        ASSERT_EQ(res[3], 0x24);
    }

    {
        DataVectorUint8 res = little_endian.get_payload_as_bigendian();
        ASSERT_EQ(res.size(), little_endian.size() * 2); // number of elements in container
        ASSERT_EQ(res[0], 0xff);
        ASSERT_EQ(res[1], 0x00);
        ASSERT_EQ(res[2], 0x42);
        ASSERT_EQ(res[3], 0x24);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// test serial connection helper stuff

TEST(RTUTestHardware, test_lowlevel_serial) {

    using namespace everest::connection;

    ASSERT_THROW(SerialDeviceConfiguration("/dev/does_not_exist"), std::runtime_error);

    SerialDeviceConfiguration serial_device_config("/dev/ttyUSB0");

    serial_device_config.set_sensible_defaults();
    // .set_baud_rate(SerialDeviceConfiguration::BaudRate::Baud_19200);
    // .set_stop_bits(SerialDeviceConfiguration::StopBits::One )
    // .set_parity( SerialDeviceConfiguration::Parity::None )
    // .set_data_bits( SerialDeviceConfiguration::DataBits::Bit_8 );

    using CFlagType = decltype(termios::c_cflag);

    ASSERT_EQ(cfgetispeed(&serial_device_config.m_tty_config), static_cast<speed_t>(B19200));
    ASSERT_EQ(cfgetospeed(&serial_device_config.m_tty_config), static_cast<speed_t>(B19200));

    ASSERT_EQ(serial_device_config.m_tty_config.c_cflag & CS8, static_cast<CFlagType>(CS8));       // 8 databits
    ASSERT_EQ(serial_device_config.m_tty_config.c_cflag & CSTOPB, static_cast<CFlagType>(0));      // stop bits
    ASSERT_EQ(serial_device_config.m_tty_config.c_cflag & PARENB, static_cast<CFlagType>(PARENB)); // parity

    SerialDevice serial_device(serial_device_config);

    ASSERT_EQ(serial_device.get_serial_device_config().m_tty_config, serial_device_config.m_tty_config);

    serial_device.open();

    // Test starts here.
    // test data stolen from:
    // bsmtool --trace --device /dev/ttyUSB0 get common
    const unsigned char request_common_model[]{0x2A, 0x03, 0x9C, 0x44, 0x00, 0x42, 0xAD, 0xA5};

    // writeToDevice goes to connection object?
    auto bytes_written = serial_device.write(request_common_model, sizeof(request_common_model));

    ASSERT_EQ(bytes_written, sizeof(request_common_model)); // the common model has this size on this device...

    std::vector<unsigned char> readbuffer(::everest::modbus::consts::rtu::MAX_ADU);
    ::size_t bytes_read = serial_device.read(readbuffer.data(), readbuffer.size());

    EXPECT_EQ(bytes_read, (unsigned)137); // the common model has this size on this device...
    ASSERT_GT(bytes_read, (unsigned)0);   // does not make sense to continue testing if we dont read anything...
    ASSERT_LE(bytes_read, ::everest::modbus::consts::rtu::MAX_ADU); // make sure we dont read past the end.

    serial_device.close();
}

TEST(RTUTestHardware, test_lowlevel_serial_error) {

    using namespace everest::connection;

    SerialDeviceConfiguration cfg;
    cfg.set_sensible_defaults();

    SerialDevice serial_device(cfg);

    serial_device.close();

    unsigned char buffer[42]{};

    // test on invalid fd
    ASSERT_THROW(serial_device.write(buffer, sizeof(buffer)), std::runtime_error);
    ASSERT_THROW(serial_device.read(buffer, sizeof(buffer)), std::runtime_error);
}

TEST(RTUTestHardware, test_read_holding_register) {

    // test the Modbus::read_holding_register to read sunspec get common model on real hardware.
    // Please keep in mind that this test likely to fail if other hardware than powemeter "BSM-WS36A-H01-1311-0000" from
    // Gebr. Bauer GbR is used. ;)
    using namespace everest::connection;

    SerialDeviceConfiguration cfg("/dev/ttyUSB0");
    cfg.set_sensible_defaults();

    SerialDevice serial_device(cfg);

    using DataVector = std::vector<unsigned char>;

    DataVector outgoing_rtu_get_common{0x2A, 0x03, 0x9C, 0x44, 0x00, 0x42, 0xAD, 0xA5};
    DataVector incomming_rtu_response{
        0x2A, 0x03, 0x84, 0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69,
        0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42,
        0x53, 0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33, 0x32, 0x43,
        0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30, 0x31, 0x39, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00, 0xc8, 0x56};
    DataVector stripped_incomming_rtu_response{
        0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69, 0x63, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x53,
        0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33,
        0x32, 0x43, 0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30,
        0x31, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00};

    RTUConnection connection(serial_device);

    everest::modbus::ModbusRTUClient client(connection);
    DataVector result =
        client.read_holding_register(42,    // device address
                                     40004, // register address
                                     66,    // number of regs to read
                                     false  // for unknown reasons: if this is false, the raw message is returned.
        );

    ASSERT_EQ(result, incomming_rtu_response);
}

class MockSerialDevice : public ::everest::connection::SerialDevice {

public:
    MOCK_METHOD(void, open, ());
    MOCK_METHOD(void, close, ());
    MOCK_METHOD(everest::connection::SerialDeviceConfiguration&, get_serial_device_config, ());
    MOCK_METHOD(void, update_timeout_configuration, (unsigned int));
    MOCK_METHOD(::size_t, write, (const unsigned char* const buffer, ::size_t count));
    MOCK_METHOD(::size_t, read, (unsigned char* buffer, ::size_t count));
};

TEST(RTUClientTest, test_rtu_client_read_holding_register) {

    // write a sunspec get_common request and receive the response
    // this is done twice to check the "unpacking" of the response ( stripping the protocol part from the response data
    // ).

    using namespace ::everest::connection;

    using ::testing::_; // this is the wildcard parameter
    using ::testing::Args;
    using ::testing::DoAll;
    using ::testing::ElementsAreArray;
    using ::testing::NiceMock;
    using ::testing::Return;
    using ::testing::SetArrayArgument;

    // RTUConnectionConfiguration config;
    // https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md#knowing-when-to-expect
    // We dont want warnings about unineresting calls, so we use the NiceMock here
    NiceMock<MockSerialDevice> serial_device;

    using DataVector = std::vector<unsigned char>;

    // outgoing message:
    // see test_crc16
    // 0x2A Address   --> bsm default address is 42 / 0x2A
    // 0x03 Function  --> read holding register
    // 0x9C data      --> Starting address Hi --> 40004 / maps to datamodel 40003
    // 0x44 data      --> Starting address Lo
    // 0x00 data      --> Number of points Hi --> read 66 points, which is the payload length of "Standard SunSpec model
    // with general information", model number 1 0x42 data      --> Number of points Lo 0xAD crc16     --> Error check
    // 0x42 crc16     --> Error check
    DataVector outgoing_rtu_get_common{0x2A, 0x03, 0x9C, 0x44, 0x00, 0x42, 0xAD, 0xA5};
    DataVector incomming_rtu_response{
        0x2A, 0x03, 0x84, 0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69,
        0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42,
        0x53, 0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33, 0x32, 0x43,
        0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30, 0x31, 0x39, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00, 0xc8, 0x56};
    DataVector stripped_incomming_rtu_response{
        0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69, 0x63, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x53,
        0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33,
        0x32, 0x43, 0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30,
        0x31, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00};

    // https://google.github.io/googletest/reference/actions.html
    // https://google.github.io/googletest/reference/matchers.html

    EXPECT_CALL(serial_device, read(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(incomming_rtu_response.begin(), incomming_rtu_response.end()),
                        Return(incomming_rtu_response.size())))
        .WillOnce(DoAll(SetArrayArgument<0>(incomming_rtu_response.begin(), incomming_rtu_response.end()),
                        Return(incomming_rtu_response.size())));

    // Args<N1, N2, ..., Nk>(m)	The tuple of the k selected (using 0-based indices) arguments matches m, e.g. Args<1,
    // 2>(Eq()) Args is a *Matcher* !!!
    EXPECT_CALL(serial_device, write(_,
                                     _ // c style array here.
                                     ))
        .With(::testing::Args<0, 1>(ElementsAreArray(
            outgoing_rtu_get_common))) // Seems that c style arrays have their own style of matching parameters
        .WillOnce(Return(outgoing_rtu_get_common.size()))
        .WillOnce(Return(outgoing_rtu_get_common.size()));

    RTUConnection connection(serial_device);

    everest::modbus::ModbusRTUClient client(connection);
    {
        DataVector result =
            client.read_holding_register(42,    // device address
                                         40004, // register address
                                         66,    // number of regs to read
                                         false  // for unknown reasons: if this is false, the raw message is returned.
            );

        // a note on comparison: the raw result has the size of the max adu of a rtu message, which in this case is
        // larger than the size of incomming_rtu_response result.resize( incomming_rtu_response.size() ); // resize,
        // otherwise we would have to compare items "by hand"
        ASSERT_EQ(result, incomming_rtu_response);
    }

    {
        DataVector result =
            client.read_holding_register(42,    // device address
                                         40004, // register address
                                         66,    // number of regs to read
                                         true   // for unknown reasons: if this is false, the raw message is returned.
            );

        ASSERT_EQ(result.size(), stripped_incomming_rtu_response.size());
        ASSERT_EQ(result, stripped_incomming_rtu_response);
    }
}

TEST(RTUClientTest, test_rtu_client_write_multiple_register) {

    // test the writer_multiple_registers
    // TODO: the mocking is on a to high level. Will have to make the serial_connection_helper stuff mockable.

    using namespace ::everest::modbus;
    using namespace ::everest::connection;

    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::ElementsAreArray;
    using ::testing::NiceMock;
    using ::testing::Return;
    using ::testing::SetArrayArgument;

    // stolen from modbus spec
    DataVectorUint8 outgoing_rtu_write_multiple_register{
        0x2A, // unit id
        0x10, // write multiple register
        0x00, // start address hi
        0x01, // start address lo
        0x00, // quantity of registers hi
        0x02, // quantity of registers lo
        0x04, // byte count
        0x00, // reg0 hi
        0x0a, // reg0 lo
        0x01, // reg1 hi
        0x02, // reg1 lo

        0x1C,
        0xd4 // crc16
    };

    ModbusDataContainerUint16 payload(ByteOrder::LittleEndian, {0x000a, 0x0102});

    DataVectorUint8 incomming_rtu_response{0x2A, // unit id
                                           0x10,
                                           0x00, // start address hi
                                           0x01, // start address lo
                                           0x00, // quantity of registers hi
                                           0x02, // quantity of registers lo
                                           0x16, // crc16
                                           0x13};

    NiceMock<MockSerialDevice> serial_device;
    EXPECT_CALL(serial_device, read(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(incomming_rtu_response.begin(), incomming_rtu_response.end()),
                        Return(incomming_rtu_response.size())));

    EXPECT_CALL(serial_device, write(_,
                                     _ // c style array here.
                                     ))
        .With(::testing::Args<0, 1>(
            ElementsAreArray(outgoing_rtu_write_multiple_register))) // Seems that c style arrays have their own style
                                                                     // of matching parameters
        .WillOnce(Return(outgoing_rtu_write_multiple_register.size()));

    RTUConnection connection(serial_device);

    ModbusRTUClient client(connection);

    DataVectorUint8 response =
        client.write_multiple_registers(0x2a,    // unit id
                                        0x0001,  // start address
                                        0x02,    // number of register to write
                                        payload, // errors will be reported by exception std::runtime_error
                                        false    // return full response
        );

    ASSERT_EQ(response, incomming_rtu_response);
}

TEST(RTUClientTest, test_rtu_client_crc_error) {

    // test if an exception is thrown on crc error

    using namespace ::everest::modbus;
    using namespace ::everest::connection;

    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::ElementsAreArray;
    using ::testing::NiceMock;
    using ::testing::Return;
    using ::testing::SetArrayArgument;

    // stolen from modbus spec, write a writer_multiple_registers request to the device
    DataVectorUint8 outgoing_rtu_write_multiple_register{
        0x2A, // unit id
        0x10, // write multiple register
        0x00, // start address hi
        0x01, // start address lo
        0x00, // quantity of registers hi
        0x02, // quantity of registers lo
        0x04, // byte count
        0x00, // reg0 hi
        0x0a, // reg0 lo
        0x01, // reg1 hi
        0x02, // reg1 lo

        0x1C,
        0xd4 // crc16
    };

    ModbusDataContainerUint16 payload(ByteOrder::LittleEndian, {0x000a, 0x0102});

    DataVectorUint8 incomming_rtu_response{0x2A, // unit id
                                           0x10,
                                           0x00, // start address hi
                                           0x01, // start address lo
                                           0x00, // quantity of registers hi
                                           0x02, // quantity of registers lo

                                           0x00, // wrong crc16, this should trigger an exception.
                                           0x00};

    NiceMock<MockSerialDevice> serial_device;

    EXPECT_CALL(serial_device, read(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(incomming_rtu_response.begin(), incomming_rtu_response.end()),
                        Return(incomming_rtu_response.size())));

    EXPECT_CALL(serial_device, write(_,
                                     _ // c style array here.
                                     ))
        .With(::testing::Args<0, 1>(
            ElementsAreArray(outgoing_rtu_write_multiple_register))) // Seems that c style arrays have their own style
                                                                     // of matching parameters
        .WillOnce(Return(outgoing_rtu_write_multiple_register.size()));

    // RTUConnection connection( config, serial_device );
    RTUConnection connection(serial_device);

    ModbusRTUClient client(connection);

    ASSERT_THROW(client.write_multiple_registers(0x2a, 0x0001, 0x02, payload, false),
                 std::runtime_error); // exception thrown on crc error
}

TEST(RTUClientTest, test_rtu_client_error_responses) {

    // we "send" a valid request "outside", and have a error response return.
    // we test if an exception (std::runtime_error) is thrown on error responses.

    using namespace ::everest::modbus;
    using namespace ::everest::connection;

    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::ElementsAreArray;
    using ::testing::NiceMock;
    using ::testing::Return;
    using ::testing::SetArrayArgument;

    // stolen from modbus spec
    DataVectorUint8 outgoing_rtu_write_multiple_register{
        0x2A, // unit id
        0x10, // write multiple register
        0x00, // start address hi
        0x01, // start address lo
        0x00, // quantity of registers hi
        0x02, // quantity of registers lo
        0x04, // byte count
        0x00, // reg0 hi
        0x0a, // reg0 lo
        0x01, // reg1 hi
        0x02, // reg1 lo

        0x1C,
        0xd4 // crc16
    };

    DataVectorUint8 incomming_rtu_error_response{0x2A,  // unit id
                                                 0x90,  // error code for 0x10 (write multiple register)
                                                 0x04}; // some valid error exception code.

    ModbusDataContainerUint16 payload(ByteOrder::LittleEndian, {0x000a, 0x0102});

    NiceMock<MockSerialDevice> serial_device;

    RTUConnection connection(serial_device);

    EXPECT_CALL(serial_device, read(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(incomming_rtu_error_response.begin(), incomming_rtu_error_response.end()),
                        Return(incomming_rtu_error_response.size())));

    EXPECT_CALL(serial_device, write(_,
                                     _ // c style array here.
                                     ))
        .With(::testing::Args<0, 1>(
            ElementsAreArray(outgoing_rtu_write_multiple_register))) // Seems that c style arrays have their own style
                                                                     // of matching parameters
        .WillOnce(Return(outgoing_rtu_write_multiple_register.size()));

    ModbusRTUClient client(connection);

    ASSERT_THROW(client.write_multiple_registers(0x2a,    // unit id
                                                 0x0001,  // start address
                                                 0x02,    // number of register to write
                                                 payload, // errors will be reported by exception std::runtime_error
                                                 false    // return full response
                                                 ),
                 std::runtime_error);
}

TEST(RTUClientTest, test_rtu_message_size_error) {

    // test throw when trying to read more register than allowed by modbus protocol
    using namespace ::everest::modbus;
    using namespace ::everest::connection;

    ::testing::NiceMock<MockSerialDevice> serial_device;
    RTUConnection connection(serial_device);
    ModbusRTUClient client(connection);

    EXPECT_THROW(client.read_holding_register(42, 40000, everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE + 1),
                 ::everest::modbus::exceptions::message_size_exception);

    // some nonsense payload
    ModbusDataContainerUint16 payload(ByteOrder::LittleEndian, {0x000a, 0x0102});
    EXPECT_THROW(client.write_multiple_registers(42, 40000, everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE + 1,
                                                 payload, true),
                 ::everest::modbus::exceptions::message_size_exception);
}

TEST(RTUClientTest, test_response_without_protocol_data) {

    // test the unpacking of response data ( stripping the protocol part from the response data )

    using namespace everest::modbus;

    // This is a read holding request. till now, response_without_protocol_data till now assumes that the data is
    // processes ist a read_holding_register communication. The data returning from response_without_protocol_data now
    // includes the number of bytes field of the response. byte count here 132 / 0x84
    DataVectorUint8 dv_raw_resrponse{
        0x2A, 0x03, 0x84, 0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69,
        0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42,
        0x53, 0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33, 0x32, 0x43,
        0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30, 0x31, 0x39, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00, 0xc8, 0x56};
    DataVectorUint8 dv_stripped_response{
        0x42, 0x41, 0x55, 0x45, 0x52, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69, 0x63, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x53,
        0x4D, 0x2D, 0x57, 0x53, 0x33, 0x36, 0x41, 0x2D, 0x48, 0x30, 0x31, 0x2D, 0x31, 0x33, 0x31, 0x31, 0x2D,
        0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x39, 0x3A, 0x33,
        0x32, 0x43, 0x41, 0x3A, 0x41, 0x46, 0x46, 0x34, 0x00, 0x00, 0x00, 0x32, 0x31, 0x30, 0x37, 0x30, 0x30,
        0x31, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x80, 0x00};

    ASSERT_EQ(ModbusRTUClient::response_without_protocol_data(dv_raw_resrponse, 0x84), dv_stripped_response);
}
