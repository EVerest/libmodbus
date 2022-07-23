#ifndef SERIAL_CONNECTION_HELPER_H_
#define SERIAL_CONNECTION_HELPER_H_

// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <string>
#include <termios.h>

namespace everest {
    namespace connection {
        namespace serial_connection_helper {

            int openSerialDevice( const std::string device );
            int closeSerialDevice( int serial_port_fd );
            void getConfiguration( int serial_port_fd, termios* tty );
            void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ );
            void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds );
            void configureDevice( int serial_port_fd, termios* tty );
            ::size_t writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count );
            ::size_t readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config );

        }
    }
}

#endif // SERIAL_CONNECTION_HELPER_H_