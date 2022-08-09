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


#include "modbus_transport.hpp"

#include "crc16.hpp"
#include "endian_tools.hpp"

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
    ModbusMessages::DataVector expected_payload { 2,3,4 };
    ASSERT_EQ( expected_payload , res.response_data() );

}
