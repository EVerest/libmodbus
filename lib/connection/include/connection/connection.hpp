// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <termios.h>

// TODO: move this into a more beautiful place... the class SerialDevice is not a helper anymore.
#include <connection/serial_connection_helper.hpp>


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
        // result of receive_bytes is a vector that has the size of received bytes
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

        ////////////////////////////////////////////////////////////////////////////////
        //
        // RTUConnection assumes that there is only *one* device on the serial line, there
        // is no collision handling implemented.

        class RTUConnection : public Connection {
        private:

            ::everest::connection::SerialDevice& m_serial_device;

        public:

            explicit RTUConnection( ::everest::connection::SerialDevice& serialDevice );
            ~RTUConnection() { m_serial_device.close(); }
            virtual int make_connection() override; // throws derived from std::runtime_error
            virtual int close_connection() override;
            virtual int send_bytes(const std::vector<uint8_t>& bytes_to_send) override; // throws derived from std::runtime_error
            virtual std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes) override; // throws derived from std::runtime_error
            virtual bool is_valid() const override;
        };


 } // namespace connection
}; // namespace everest
