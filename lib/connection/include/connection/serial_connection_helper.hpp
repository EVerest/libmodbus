// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef SERIAL_CONNECTION_HELPER_H_
#define SERIAL_CONNECTION_HELPER_H_

#include <string>
#include <termios.h>
#include <cstdint>

#include <type_traits>
#include <iostream>

namespace everest {
    namespace connection {

        struct SerialDeviceConfiguration {

            termios m_tty_config {};
            std::string m_device;

            enum struct InitFromDevice {
                DoInit, DontInit
            };

            unsigned int initial_read_timeout_deciseconds { 50 }; // wait for first input
            unsigned int default_read_timeout_deciseconds { 2 };  // max wait during reading / wait for end of transmission

            explicit SerialDeviceConfiguration( std::string device ); // throws if the device could not be opened.

            SerialDeviceConfiguration() = default;

            enum struct BaudRate {
                Baud_0       = B0  ,
                Baud_50      = B50  ,
                Baud_75      = B75  ,
                Baud_110     = B110  ,
                Baud_134     = B134  ,
                Baud_150     = B150  ,
                Baud_200     = B200  ,
                Baud_300     = B300  ,
                Baud_600     = B600  ,
                Baud_1200    = B1200  ,
                Baud_1800    = B1800  ,
                Baud_2400    = B2400  ,
                Baud_4800    = B4800  ,
                Baud_9600    = B9600  ,
                Baud_19200   = B19200  ,
                Baud_38400   = B38400  ,
                Baud_57600   = B57600  ,
                Baud_115200  = B115200  ,
                Baud_230400  = B230400  ,
                Baud_460800  = B460800  ,
                Baud_500000  = B500000  ,
                Baud_576000  = B576000  ,
                Baud_921600  = B921600  ,
                Baud_1000000 = B1000000  ,
                Baud_1152000 = B1152000  ,
                Baud_1500000 = B1500000  ,
                Baud_2000000 = B2000000  ,
            };

            enum struct DataBits {
                Bit_8 = CS8,
                Bit_7 = CS7,
                Bit_6 = CS6,
                Bit_5 = CS5
            };

            enum struct StopBits {
                One,
                Two
            };

            enum struct Parity {
                None,
                Even,
                Odd
            };

            void get_current_config();

            SerialDeviceConfiguration& set_baud_rate( BaudRate );
            SerialDeviceConfiguration& set_data_bits( DataBits );
            SerialDeviceConfiguration& set_stop_bits( StopBits );
            SerialDeviceConfiguration& set_parity( Parity );

            // set cread, clocal, disable canonical, disable echo, disable signal chars
            SerialDeviceConfiguration& set_sensible_defaults();

        };

        static_assert( std::is_copy_constructible<SerialDeviceConfiguration>::value, "SerialDeviceConfiguration needs to be copy constructible!" );

        class SerialDevice{

            int m_fd = -1;
            SerialDeviceConfiguration m_serial_device_configuration;

        protected:

            SerialDevice() {}

        public:

            explicit SerialDevice( const SerialDeviceConfiguration& serialDeviceConfiguration ) :
                m_serial_device_configuration( serialDeviceConfiguration ) {}


            virtual void open();
            virtual void close();
            virtual SerialDeviceConfiguration& get_serial_device_config();

            virtual ::size_t write( const unsigned char* const buffer, ::size_t count );
            virtual ::size_t read( unsigned char* buffer, ::size_t count );

        };

        // can be used to record conversations with real hardware for later usage in tests.
        class SerialDeviceLogToStream : public SerialDevice {

        protected:

            std::ostream* m_stream;

        public:

            explicit SerialDeviceLogToStream( const SerialDeviceConfiguration& serialDeviceConfiguration ,
                                     std::ostream* stream = &std::clog ) :
                SerialDevice( serialDeviceConfiguration ),
                m_stream( stream ) {}

            virtual ::size_t write( const unsigned char* const buffer, ::size_t count ) override;
            virtual ::size_t read( unsigned char* buffer, ::size_t count ) override;

            void set_stream( std::ostream* stream ) {
                m_stream = stream ;
            }

        };

   }
}

bool operator == ( const termios& lhs, const termios& rhs );

#endif // SERIAL_CONNECTION_HELPER_H_
