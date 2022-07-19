// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <termios.h>

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
            virtual bool is_valid() const = 0;
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
            bool is_valid() const;
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
            bool is_valid() const;
    };

       struct RTUConnectionConfiguration {

            std::string m_device = "/dev/ttyUSB0";
            // std::string m_baud = "19200"; //hardcoded for now
            unsigned int m_initial_wait_deciseconds = 50; // wait 5 secs for response
        };

        ////////////////////////////////////////////////////////////////////////////////
        //
        // RTUConnection assumes that there is only *one* device on the serial line, there
        // is not collision handling implemented.

        class RTUConnection : public Connection {
        private:

            const RTUConnectionConfiguration m_config;
            int m_fd = -1;
            termios m_tty_config {};

        protected:

            RTUConnection() {}

        public:

            RTUConnection( const RTUConnectionConfiguration& );
            virtual int make_connection() override;
            virtual int close_connection() override;
            virtual int send_bytes(const std::vector<uint8_t>& bytes_to_send) override;
            virtual std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes) override;
            virtual bool is_valid() const override;
        };


 } // namespace connection
}; // namespace everest
