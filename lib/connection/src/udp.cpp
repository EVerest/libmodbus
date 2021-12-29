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

#include <connection/connection.hpp>
#include <connection/utils.hpp>
#include <connection/exceptions.hpp>

using namespace everest::connection;

UDPConnection::UDPConnection(const std::string& address_, const int& port_) : address(address_), port(port_), socket_fd(-1) {
    make_connection();
}

UDPConnection::~UDPConnection() {
    close_connection();
}

int UDPConnection::make_connection() {

    /* NOTE: UDP is connectionless, so this function just operates by opening a socket locally through the 'socket' syscall. The recipient is not involved in this step.
     * The make_connection() and close_connection() methods are implemented just to setup things locally and to comply with the proposed interface. One should bear that in mind while using this class. */

    // Opening socket locally
    EVLOG(debug) << "Attempting to create UDP socket connection with endpoint " << address << ":" << port << ".";
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        std::stringstream error_message;
        error_message << "UDP Socket creation error: fd = " << socket_fd;
        throw exceptions::udp::udp_socket_error(error_message.str());
    }
    EVLOG(debug) << "Successfully opened local UDP socket";

    // Setting up address struct
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr( address.c_str() );

    connection_status = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        std::stringstream error_message;
        error_message << "UDP socket open failed. fd = " << socket_fd;
        EVLOG(error) << error_message.str();
        throw exceptions::udp::udp_socket_error(error_message.str());
    }

    return connection_status;
}

int UDPConnection::close_connection() {

    EVLOG(debug) << "Attempting to close UDP socket with fd = " << socket_fd << ".";
    int close_status = close(socket_fd);

    if (close_status == -1) {
        std::stringstream error_message;
        error_message << "Failed to close UDP socket with fd = " << socket_fd << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::udp::udp_socket_error(error_message.str());
    }
    EVLOG(debug) << "Closed UDP socket with fd = " << socket_fd << ".";
    socket_fd = -1;
    connection_status = -1;
    return close_status;
}

const bool UDPConnection::is_valid() const {
    if (connection_status == -1 || socket_fd == -1)
        return false;
    return true;
}

int UDPConnection::send_bytes(const std::vector<uint8_t>& bytes_to_send) {

    if (!is_valid()) {
        std::stringstream error_message;
        error_message << "MODBUS UDP - No connection established with " << address << ":" << port << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::udp::udp_socket_error(error_message.str());
    }

    int message_len = bytes_to_send.size();
    EVLOG(debug) << "Attempting to send message to " << address << ":" << port << " - " << utils::get_bytes_hex_string(bytes_to_send) << "- Size = " << message_len;

    // Trying to send
    int bytes_sent = sendto(socket_fd, (unsigned char*) bytes_to_send.data(), message_len, 0, (struct sockaddr*) NULL, sizeof(struct sockaddr));
    if (bytes_sent == -1) {
        std::stringstream error_message;
        error_message << "MODBUS UDP - Error while sending message: " << bytes_to_send.data();
        EVLOG(error) << error_message.str();
        throw exceptions::communication_error(error_message.str());
    }

    EVLOG(debug) << "Successfully sent " << bytes_sent << " bytes.";
    return bytes_sent;
}

std::vector<uint8_t> UDPConnection::receive_bytes(unsigned int number_of_bytes) {

    if (!is_valid()) {
        std::stringstream error_message;
        error_message << "No connection established with " << address << ":" << port << ".";
        EVLOG(error) << error_message.str();
        throw exceptions::udp::udp_socket_error(error_message.str());
    }

    // Attempting to receive
    std::vector<uint8_t> received_bytes;
    received_bytes.reserve(number_of_bytes);
    uint8_t response_buffer[number_of_bytes];

    int num_bytes_received = recvfrom(socket_fd, &response_buffer, sizeof(response_buffer), 0, (struct sockaddr*) NULL, NULL);
    if (num_bytes_received == -1) {
        EVLOG(error) << "No bytes received from " << address << ":" << port << ". Closing connection and returning preallocated buffer.";
        close_connection();
        return received_bytes;
    }

    received_bytes.assign(response_buffer, response_buffer+num_bytes_received);
    EVLOG(debug) << received_bytes.size() << " bytes received from " << address << ":" << port << " - " << utils::get_bytes_hex_string(received_bytes);
    return received_bytes;
}

