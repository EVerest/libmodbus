// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <stdio.h>
#include <stdlib.h>

#include <modbus/exceptions.hpp>
#include <modbus/utils.hpp>

#include <consts.hpp>

namespace everest {
namespace modbus {

std::vector<uint8_t> utils::ip::make_mbap_header(uint16_t message_length, uint8_t unit_id) {

    // Header buffer
    std::vector<uint8_t> mbap_header(consts::tcp::MBAP_HEADER_LENGTH);

    // Generating random 2 byte transaction ID
    // uint16_t transac tion_id = 1;
    srand(time(0));
    uint16_t transaction_id = rand() % std::numeric_limits<uint16_t>::max();

    // Adding transaction ID bytes
    mbap_header[0] = (transaction_id >> 8) & 0xFF;
    mbap_header[1] = transaction_id & 0xFF;

    // Adding protocol ID bytes
    mbap_header[2] = (consts::tcp::PROTOCOL_ID >> 8) & 0xFF;
    mbap_header[3] = consts::tcp::PROTOCOL_ID & 0xFF;

    // Adding message length bytes
    mbap_header[4] = (message_length >> 8) & 0xFF;
    mbap_header[5] = message_length & 0xFF;

    // Adding unit/slave ID
    mbap_header[6] = unit_id;

    return mbap_header;
}

std::vector<uint8_t> utils::build_read_command_message_body(std::uint8_t function_code, uint16_t first_register_address,
                                                            uint16_t num_registers_to_read) {

    std::vector<uint8_t> message_body(consts::READ_REGISTER_COMMAND_LENGTH - 1);

    // Adding read function code
    message_body[0] = function_code;

    // Adding first register data address
    message_body[1] = (first_register_address >> 8) & 0xFF;
    message_body[2] = first_register_address & 0xFF;

    // Adding requested register number
    message_body[3] = (num_registers_to_read >> 8) & 0xFF;
    message_body[4] = num_registers_to_read & 0xFF;

    return message_body;
}

std::vector<uint8_t> utils::build_read_holding_register_message_body(uint16_t first_register_address,
                                                                     uint16_t num_registers_to_read) {
    return build_read_command_message_body(consts::READ_HOLDING_REGISTER_FUNCTION_CODE, first_register_address,
                                           num_registers_to_read);
}

std::vector<uint8_t> utils::build_read_input_register_message_body(uint16_t first_register_address,
                                                                   uint16_t num_registers_to_read) {
    return build_read_command_message_body(consts::READ_INPUT_REGISTER_FUNCTION_CODE, first_register_address,
                                           num_registers_to_read);
}

std::vector<uint8_t>
utils::build_write_multiple_register_body(uint16_t first_register_address, uint16_t num_registers_to_write,
                                          const ::everest::modbus::ModbusDataContainerUint16& payload) {

    std::vector<uint8_t> message_body;
    message_body.reserve(1 + // function code
                         2 + // starting address
                         2 + // quantity of registers
                         payload.size());

    message_body.push_back(0x10); // function code

    // first register address
    message_body.push_back((first_register_address >> 8) & 0xff); // hibyte
    message_body.push_back(first_register_address & 0xff);        // lowbyte

    // number of registers to write
    message_body.push_back((num_registers_to_write >> 8) & 0xff); // hibyte
    message_body.push_back(num_registers_to_write & 0xff);        // lowbyte

    // byte count: for now only 16 bit register, so bytecount is num_registers_to_write * 2
    message_body.push_back(num_registers_to_write * 2);
    std::vector<uint8_t> payload_big_endian = payload.get_payload_as_bigendian();
    std::copy(payload_big_endian.cbegin(), payload_big_endian.cend(), std::back_inserter(message_body));

    return message_body;
}

uint16_t utils::ip::check_mbap_header(const std::vector<uint8_t>& sent_message,
                                      const std::vector<uint8_t>& received_message) {

    // Validating echoed transaction ID
    bool transaction_id_match = (sent_message[0] == received_message[0] && sent_message[1] == received_message[1]);
    if (!transaction_id_match)
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received transaction ID's do not match.");

    // Validating echoed protocol ID
    bool protocol_id_match = (sent_message[2] == received_message[2] && sent_message[3] == received_message[3]);
    if (!protocol_id_match)
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received protocol ID's do not match.");

    // Validating echoed unit id
    bool unit_id_match = (sent_message[6] == received_message[6]);
    if (!unit_id_match)
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received unit ID's do not match."
                                             "");

    // Validating echoed function code
    bool function_code_match = (sent_message[7] == received_message[7]);
    if (!function_code_match)
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received function codes do not match."
                                             "");

    // Extracting number of bytes to follow
    uint16_t number_of_following_bytes = 0;
    number_of_following_bytes = (received_message[4] << 8) | received_message[5];

    return number_of_following_bytes;
}

std::vector<uint8_t> utils::extract_body_from_response(const std::vector<uint8_t>& response, int num_data_bytes) {
    std::vector<uint8_t> response_body(response.end() - num_data_bytes, response.end());
    return response_body;
}

std::vector<uint8_t> utils::extract_register_bytes_from_response(const std::vector<uint8_t>& response,
                                                                 int num_data_bytes) {
    return utils::extract_registers_bytes_from_response_body(response);
}

std::vector<uint8_t> utils::extract_registers_bytes_from_response_body(const std::vector<uint8_t>& response_body) {
    uint8_t num_register_bytes = response_body.at(2);
    std::vector<uint8_t> register_bytes =
        std::vector<uint8_t>(response_body.begin() + 3, response_body.begin() + 3 + num_register_bytes);
    return register_bytes;
}

void utils::print_message_hex(const std::vector<uint8_t>& message) {
    for (int n : message) {
        printf("%.2X ", n);
    }
    printf("\n");
}

void utils::print_message_first_N_bytes(unsigned char* message, int N) {
    for (int i = 0; i < N; i++) {
        printf("%.2X ", message[i]);
    }
    printf("\n");
}

utils::CRCResultType utils::calcCRC_16_ANSI(const utils::PayloadType* payload, std::size_t payload_length) {

    // https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Polynomial_representations_of_cyclic_redundancy_checks
    // implementation stolen from: https://modbus.org/docs/PI_MBUS_300.pdf

    // High-Order Byte Table
    /* Table of CRC values for high–order byte */
    const utils::PayloadType auchCRCHi[] = {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40};

    /* Table of CRC values for low–order byte */
    const utils::PayloadType auchCRCLo[] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C,
        0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
        0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17,
        0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
        0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
        0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21,
        0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
        0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8,
        0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53,
        0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
        0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E,
        0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
        0x41, 0x81, 0x80, 0x40};

    utils::PayloadType uchCRCHi = 0xff;
    utils::PayloadType uchCRCLo = 0xff;

    while (payload_length--) {
        std::size_t uIndex = uchCRCHi ^ *payload++;
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }

    return (uchCRCHi << 8 | uchCRCLo);
}
} // namespace modbus
} // namespace everest
