// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <consts.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <modbus/exceptions.hpp>
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>
#include <sstream>
#include <string>

using namespace everest::modbus;

/*
 * Note on implemetation:
 * It is known that we often copy data from one vector to the other ( full_message_from_body, helper function in utils )
 * This is by design of the older parts of this library.
 */

ModbusRTUClient::ModbusRTUClient(connection::Connection& conn_) : ModbusClient(conn_) {
}

ModbusRTUClient::~ModbusRTUClient() {
    conn.close_connection();
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

union SwapIt {
    uint16_t value_16;
    struct {
        unsigned char low;
        unsigned char hi;
    } value_8;
};

#endif

DataVectorUint8 ModbusRTUClient::response_without_protocol_data(const DataVectorUint8& raw_response,
                                                                std::size_t payload_length) {
    // strip address and function bytes, but include bytecount byte
    const int offset_protocol_bytes{3};
    return DataVectorUint8(raw_response.cbegin() + offset_protocol_bytes,
                           raw_response.cbegin() + offset_protocol_bytes + payload_length);
}

const DataVectorUint8 ModbusRTUClient::read_holding_register(uint8_t unit_id, uint16_t first_register_address,
                                                             uint16_t num_registers_to_read,
                                                             bool return_only_registers_bytes) const {

    using namespace std::string_literals;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    if (num_registers_to_read > everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE)
        throw everest::modbus::exceptions::message_size_exception(
            ""s + __PRETTY_FUNCTION__ + " Requested number of 16 bit registers " +
            std::to_string(num_registers_to_read) + " would exceed allowed message size of " +
            std::to_string(everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE) + " registers ");

    DataVectorUint8 body =
        utils::build_read_holding_register_message_body(first_register_address, num_registers_to_read);

    DataVectorUint8 full_message = full_message_from_body(body, consts::READ_HOLDING_REGISTER_MESSAGE_LENGTH, unit_id);

    conn.send_bytes(full_message);
    modbus::DataVectorUint8 response = conn.receive_bytes(max_adu_size());
    uint16_t payload_size = validate_response(response, full_message);
    return return_only_registers_bytes ? response_without_protocol_data(response, payload_size) : response;

#else

    static_assert(false, "implementation currently done for little endian only");

#endif
}

DataVectorUint8 ModbusDataContainerUint16::get_payload_as_bigendian() const {

    DataVectorUint8 result;
    result.reserve(m_payload.size() * sizeof(DataVectorUint16::value_type));

    if (m_byte_order == ByteOrder::LittleEndian)
        for (uint16_t value : m_payload) {
            SwapIt s{value};
            result.push_back(s.value_8.hi);
            result.push_back(s.value_8.low);
        }
    else
        for (uint16_t value : m_payload) {
            SwapIt s{value};
            result.push_back(s.value_8.low);
            result.push_back(s.value_8.hi);
        }

    return result;
}

DataVectorUint8 everest::modbus::ModbusRTUClient::write_multiple_registers(uint8_t unit_id,
                                                                           uint16_t first_register_address,
                                                                           uint16_t num_registers_to_write,
                                                                           const ModbusDataContainerUint16& payload,
                                                                           bool return_only_registers_bytes) const {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    using namespace std::string_literals;

    if (num_registers_to_write > everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE)
        throw everest::modbus::exceptions::message_size_exception(
            ""s + __PRETTY_FUNCTION__ + " Requested number of 16 bit registers " +
            std::to_string(num_registers_to_write) + " would exceed allowed message size of " +
            std::to_string(everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE) + " registers !");

    DataVectorUint8 body =
        utils::build_write_multiple_register_body(first_register_address, num_registers_to_write, payload);
    DataVectorUint8 full_message = full_message_from_body(body, body.size() /* unused parameter */, unit_id);

    conn.send_bytes(full_message);
    modbus::DataVectorUint8 response = conn.receive_bytes(max_adu_size());
    uint16_t payload_size = validate_response(response, full_message);
    return return_only_registers_bytes ? response_without_protocol_data(response, payload_size) : response;

#else

    static_assert(false, "implementation currently done for little endian only");

#endif
}

const everest::modbus::DataVectorUint8 ModbusRTUClient::full_message_from_body(const DataVectorUint8& body,
                                                                               uint16_t /* message_length */,
                                                                               std::uint8_t unit_id) const {

    // body now is the modbus command code and the payload.
    DataVectorUint8 full_message;          // need an empty vector
    full_message.reserve(body.size() + 1 + // unit_id
                         2);               // crc16

    full_message.push_back(unit_id);
    std::copy(body.cbegin(), body.cend(), std::back_inserter(full_message));

    SwapIt swapIt;
    swapIt.value_16 = everest::modbus::utils::calcCRC_16_ANSI(full_message.data(), full_message.size());
    full_message.push_back(swapIt.value_8.hi);
    full_message.push_back(swapIt.value_8.low);

    return full_message;
}

uint16_t everest::modbus::ModbusRTUClient::validate_response(const DataVectorUint8& response,
                                                             const DataVectorUint8& request) const {

    using namespace std::string_literals;

    if (response.size() > max_adu_size())
        throw everest::modbus::exceptions::should_never_happen(
            ""s + __PRETTY_FUNCTION__ + " response size " + std::to_string(response.size()) +
            " is larger than max allowed message size " + std::to_string(max_adu_size()) + " !");

    if (response.empty())
        throw everest::modbus::exceptions::empty_response(""s + __PRETTY_FUNCTION__ +
                                                          " response is empty, maybe timeout on reading device. ");

    // FIXME: What happens in case the request was a broadcast?
    if (response.at(0) != request.at(0))
        throw everest::modbus::exceptions::unmatched_response(""s + __PRETTY_FUNCTION__ +
                                                              " request / response unit id mismatch. ");

    if (not((response.at(1) & 0x80) == 0)) {
        uint8_t exception_code = response.at(2);
        std::stringstream ss;
        std::string error_message;
        switch (exception_code) {
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
        ss << __PRETTY_FUNCTION__ << "  response returned an error code: " << std::hex << (int)exception_code << " ( "
           << error_message << " ) ";
        throw everest::modbus::exceptions::modbus_exception(ss.str(), exception_code);
    }

    if (response.at(1) != request.at(1))
        throw everest::modbus::exceptions::unmatched_response(""s + __PRETTY_FUNCTION__ +
                                                              " request / response function id mismatch. ");

    SwapIt crcResponseCalculated;
    crcResponseCalculated.value_16 = ::everest::modbus::utils::calcCRC_16_ANSI(response.data(), response.size() - 2);

    SwapIt crcResponseData;
    crcResponseData.value_8.hi = response.at(response.size() - 2);
    crcResponseData.value_8.low = response.at(response.size() - 1);

    if (crcResponseCalculated.value_16 != crcResponseData.value_16)
        throw everest::modbus::exceptions::checksum_error(""s + __PRETTY_FUNCTION__ + " checksum error ");

    uint16_t result_size = response.at(2);

    return result_size;
}
