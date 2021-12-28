// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <string>
#include <cstdint>
#include <vector>

#include "../src/connection.hpp"

namespace everest { namespace connection {
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
