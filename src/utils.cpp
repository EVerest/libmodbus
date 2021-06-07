#include <stdio.h>
#include <limits>
#include <random>
#include <stdlib.h>
#include <iostream>

#include <modbus/utils.hpp>
#include <modbus/exceptions.hpp>

#include "consts.hpp"

using namespace everest::modbus;

std::vector<uint8_t> utils::tcp::make_mbap_header(uint16_t message_length, uint8_t unit_id) {

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

std::vector<uint8_t> utils::build_read_holding_register_message_body(uint16_t first_register_address, uint16_t num_registers_to_read) {
    // Preparing message body
    std::vector<uint8_t> message_body(consts::READ_HOLDING_REGISTER_MESSAGE_LENGTH - 1);

    // Adding read function code
    message_body[0] = consts::READ_HOLDING_REGISTER_FUNCTION_CODE;

    // Adding first register data address
    message_body[1] = (first_register_address >> 8) & 0xFF;
    message_body[2] = first_register_address & 0xFF;

    // Adding requested register number
    message_body[3] = (num_registers_to_read >> 8) & 0xFF;
    message_body[4] = num_registers_to_read & 0xFF;

    return message_body;
}

uint16_t utils::tcp::check_mbap_header(const std::vector<uint8_t>& sent_message, const std::vector<uint8_t>& received_message) {

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
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received unit ID's do not match.""");

    // Validating echoed function code
    bool function_code_match = (sent_message[7] == received_message[7]);
    if (!function_code_match)
        throw exceptions::unmatched_response("MODBUS TCP - Sent and received function codes do not match.""");

    // Extracting number of bytes to follow
    uint16_t number_of_following_bytes = 0;
    number_of_following_bytes = (received_message[4] << 8) | received_message[5];

    return number_of_following_bytes;
}

std::vector<uint8_t> utils::extract_body_from_response(const std::vector<uint8_t>& response, int num_data_bytes) {
    std::vector<uint8_t> response_body(response.end() - num_data_bytes, response.end());
    return response_body;
}

std::vector<uint8_t> utils::extract_register_bytes_from_response(const std::vector<uint8_t>& response, int num_data_bytes) {
    std::vector<uint8_t> body = utils::extract_body_from_response(response, num_data_bytes);
    return utils::extract_registers_bytes_from_response_body(body);
}

std::vector<uint8_t> utils::extract_registers_bytes_from_response_body(const std::vector<uint8_t>& response_body) {
    uint8_t unit_id = response_body[0];
    uint8_t function_code = response_body[1];
    uint8_t num_register_bytes = response_body[2];
    std::vector<uint8_t> register_bytes = std::vector<uint8_t>(response_body.end() - num_register_bytes, response_body.end());
    return register_bytes;
}

void utils::print_message_hex(const std::vector<uint8_t>& message) {
        for (int n : message) {
            printf("%.2X ", n);
        }
        printf("\n");
}

void utils::print_message_first_N_bytes(unsigned char *message, int N) {
    for (int i = 0 ; i < N ; i++) {
        printf("%.2X ", message[i]);        
    }
    printf("\n");
}