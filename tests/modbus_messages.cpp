#include "modbus_messages.hpp"
#include "endian_tools.hpp"

#include <stdexcept>
#include <string>

ModbusMessages::ReadHoldingRegisterQuery& ModbusMessages::ReadHoldingRegisterQuery::register_address_unit16( std::uint16_t read_register_address ) {

    m_register_address = read_register_address;
    return *this;
}


ModbusMessages::ReadHoldingRegisterQuery& ModbusMessages::ReadHoldingRegisterQuery::number_of_register_to_read( std::uint8_t number_of_register_to_read) {

    m_number_of_register_to_read = number_of_register_to_read;
    return *this;
}


std::size_t ModbusMessages::ReadHoldingRegisterQuery::size() const {

    return
        1 + // function code
        2 + // register address
        2;

}


bool ModbusMessages::ReadHoldingRegisterQuery::is_ready() const {

    return m_register_address.has_value() and m_number_of_register_to_read.has_value();

}


void ModbusMessages::ReadHoldingRegisterQuery::append_query_to( DataVector& outgoing ) const {

    outgoing.push_back( high_byte( m_register_address.value() ));
    outgoing.push_back( low_byte(  m_register_address.value() ));
    outgoing.push_back( high_byte( m_number_of_register_to_read.value() ));
    outgoing.push_back( low_byte(  m_number_of_register_to_read.value() ));

}


bool ModbusMessages::ModbusResponse::empty() const {
    return size_raw() == 0;
}


std::size_t ModbusMessages::ModbusResponse::size_raw() const {
    return m_data.data.size();
}


std::size_t ModbusMessages::ModbusResponse::size_payload () const {

    return
        m_data.payload_begin > m_data.payload_end ?
        0 : // the iterator seem to point somewhere into the nothing...
        size_raw() -
        ( std::distance( m_data.data.begin() , m_data.payload_begin ) +
          std::distance( m_data.payload_end  , m_data.data.end() ));

}


ModbusMessages::ResponseData& ModbusMessages::ModbusResponse::get_response_data_container() {

    return m_data;

}


ModbusMessages::DataVector ModbusMessages::ModbusResponse::response_data() const {

    DataVector result;
    result.reserve( size_payload() );
    std::copy( m_data.payload_begin, m_data.payload_end, std::back_inserter( result ) );
    return result;

}


std::uint8_t ModbusMessages::ModbusResponse::function_code() const {

    using namespace std::string_literals;

    if ( empty() )
        throw std::runtime_error( ""s + __PRETTY_FUNCTION__ + " Attempt to get function code from empty / invalid response.");

    return (*m_data.payload_begin);
}
