// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#pragma once

#include <vector>
#include <cstdint>

namespace everest { namespace connection {
    class Connection {
        private:
            Connection(const Connection&) = delete;
            Connection& operator=(const Connection&) = delete;

        protected:
            int connection_status;

        public:
            Connection() : connection_status(-1) {};
            virtual ~Connection() = default;
            virtual int make_connection() = 0;
            virtual int close_connection() = 0;
            virtual int send_bytes(const std::vector<uint8_t>& bytes_to_send) = 0;
            virtual std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes) = 0;
            virtual const bool is_valid() const = 0;
    };

    class TCPConnection : public Connection {
        private:
            int port;
            std::string address;
            int socket_fd;

        public:
            TCPConnection(const std::string& address_, const int& port_);
            ~TCPConnection();
            int make_connection();
            int send_bytes(const std::vector<uint8_t>& bytes_to_send);
            std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes);
            int close_connection();
            const bool is_valid() const;
    };

    class UDPConnection : public Connection {
        private:
            int port;
            std::string address;
            int socket_fd;

        public:
            UDPConnection(const std::string& address_, const int& port_);
            ~UDPConnection();
            int make_connection();
            int send_bytes(const std::vector<uint8_t>& bytes_to_send);
            std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes);
            int close_connection();
            const bool is_valid() const;
    };

 } // namespace connection
}; // namespace everest
