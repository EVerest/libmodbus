// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <connection/serial_connection_helper.hpp>

TEST(TestSerialHelper, testSerialHelperConfiguration) {

    using namespace everest::connection;

    SerialDeviceConfiguration config{};

    config.set_sensible_defaults()
        .set_baud_rate(SerialDeviceConfiguration::BaudRate::Baud_110)
        .set_data_bits(SerialDeviceConfiguration::DataBits::Bit_7)
        .set_stop_bits(SerialDeviceConfiguration::StopBits::One)
        .set_parity(SerialDeviceConfiguration::Parity::Even);

    EXPECT_EQ(cfgetispeed(&config.m_tty_config), B110);      // baud
    EXPECT_EQ(config.m_tty_config.c_cflag & CS7, CS7);       // databits
    EXPECT_EQ(config.m_tty_config.c_cflag & CSTOPB, 0);      // stop bits
    EXPECT_EQ(config.m_tty_config.c_cflag & PARENB, PARENB); // parity

    auto c_flags = CREAD | CLOCAL;
    EXPECT_EQ(config.m_tty_config.c_cflag & c_flags, c_flags);

    // check disabled flags
    auto l_flags = ICANON | ECHO | ECHOE | ECHONL | ISIG;
    EXPECT_EQ(config.m_tty_config.c_lflag & l_flags, 0);
}

TEST(TestSerialHelper, BaudFromInt) {

    using namespace everest::connection;

    {
        SerialDeviceConfiguration::BaudrateFromIntResult result =
            SerialDeviceConfiguration::baudrate_from_integer(19200);
        EXPECT_EQ(result.baud, SerialDeviceConfiguration::BaudRate::Baud_19200);
        EXPECT_TRUE(result.conversion_ok);
    }

    {
        SerialDeviceConfiguration::BaudrateFromIntResult result = SerialDeviceConfiguration::baudrate_from_integer(666);
        EXPECT_EQ(result.baud, SerialDeviceConfiguration::BaudRate::Baud_9600);
        EXPECT_FALSE(result.conversion_ok);
    }
}
