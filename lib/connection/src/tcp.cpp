// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <string>
#include <sstream>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <everest/logging.hpp>

#include <connection/tcp.hpp>
#include <connection/utils.hpp>
#include <connection/exceptions.hpp>

using namespace everest::connection;

TCPConnection::TCPConnection(const std::string& address_, const int& port_) : address(address_), port(port_), socket_fd(-1) {
    make_connection();
}

TCPConnection::~TCPConnection() {
    close_connection();
}

int TCPConnection::make_connection() {

    // Opening socket locally
    EVLOG(debug) << "Attempting to create TCP socket connection with endpoint " << address << ":" << port << ".";
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::stringstream error_message;
        error_message << "TCP Socket creation error while connecting to endpoint " << address << ":" << port << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::tcp::tcp_connection_error(error_message.str());
    }
    EVLOG(info) << "Successfully opened TCP socket with endpoint " << address << ":" << port << ". fd = " << socket_fd;

    // Setting up address struct
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr( address.c_str() );

    // Connecting
    connection_status = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        std::stringstream error_message;
        error_message << "TCP socket connection establishment failed while trying to reach endpoint " << address << ":" << port << ". fd = " << socket_fd;
        EVLOG(error) << error_message.str();
        throw exceptions::tcp::tcp_connection_error(error_message.str());
    }
    EVLOG(info) << "Succesfully opened TCP socket connection with endpoint " << address << ":" << port << ". fd = " << socket_fd;

    return connection_status;
}

int TCPConnection::close_connection() {
    
    EVLOG(debug) << "Attempting to close connection for socket with fd = " << socket_fd << ".";
    int close_status = close(socket_fd);
    
    if (close_status == -1) {
        std::stringstream error_message;
        error_message << "Failed to close TCP socket with fd = " << socket_fd << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::tcp::tcp_connection_error(error_message.str());
    }
    EVLOG(info) << "Closed socket with fd = " << socket_fd << ".";
    socket_fd = -1;
    connection_status = -1;
    return close_status;
}

const bool TCPConnection::is_valid() const {
    if (connection_status == -1 || socket_fd == -1)
        return false;
    return true;
}

int TCPConnection::send_bytes(const std::vector<uint8_t>& bytes_to_send) {

    if (!is_valid()) {
        std::stringstream error_message;
        error_message << "MODBUS TCP - No connection established with " << address << ":" << port << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::tcp::tcp_connection_error(error_message.str());
    }

    int message_len = bytes_to_send.size();
    EVLOG(debug) << "Attempting to send message to " << address << ":" << port << " - " << utils::get_bytes_hex_string(bytes_to_send) << "- Size = " << message_len;

    // Trying to send
    int bytes_sent = send(socket_fd, (unsigned char*) bytes_to_send.data(), message_len, 0);
    if (bytes_sent == -1) {
        std::stringstream error_message;
        error_message << "MODBUS TCP - Error while sending message: " << bytes_to_send.data();
        EVLOG(error) << error_message.str();
        throw exceptions::communication_error(error_message.str());
    }

    EVLOG(info) << "Successfully sent " << bytes_sent << " bytes.";
    return bytes_sent;
}

std::vector<uint8_t> TCPConnection::receive_bytes(unsigned int number_of_bytes) {

    if (!is_valid()) {
        std::stringstream error_message;
        error_message << "No connection established with " << address << ":" << port << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::tcp::tcp_connection_error(error_message.str());
    }

    // Attempting to receive
    std::vector<uint8_t> received_bytes;
    received_bytes.reserve(number_of_bytes);
    uint8_t response_buffer[number_of_bytes];

    int num_bytes_received = recv(socket_fd, &response_buffer, sizeof(response_buffer), 0);
    if (num_bytes_received == -1) {
        EVLOG(error) << "No bytes received from " << address << ":" << port << ". Closing connection and returning preallocated buffer.";
        close_connection();
        return received_bytes;
    }

    received_bytes.assign(response_buffer, response_buffer+num_bytes_received);
    EVLOG(info) << received_bytes.size() << " bytes received from " << address << ":" << port << " - " << utils::get_bytes_hex_string(received_bytes);
    return received_bytes;
}

