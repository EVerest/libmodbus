#ifndef SERIAL_CONNECTION_HELPER_H_
#define SERIAL_CONNECTION_HELPER_H_

// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <string>
#include <termios.h>

namespace everest {
    namespace connection {

        class SerialDevice {

        public:

            virtual int openSerialDevice( const std::string device );
            virtual int closeSerialDevice( int serial_port_fd );
            virtual void getConfiguration( int serial_port_fd, termios* tty );
            virtual void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ );
            virtual void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds );
            virtual void configureDevice( int serial_port_fd, termios* tty );
            virtual ::size_t writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count );
            virtual ::size_t readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config );

        };

   }
}

#endif // SERIAL_CONNECTION_HELPER_H_
