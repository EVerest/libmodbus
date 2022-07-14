// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_EXCEPTIONS_H
#define MODBUS_EXCEPTIONS_H

#include <stdexcept>

namespace everest {

    namespace modbus {

        namespace exceptions {

            class message_size_exception : public std::runtime_error {
            public:
                explicit message_size_exception(const std::string& what_arg) : std::runtime_error(what_arg) {}
            };
    
            class unmatched_response : public std::runtime_error {
            public:
                explicit unmatched_response(const std::string& what_arg) : std::runtime_error(what_arg) {}
            };

            class checksum_error : public std::runtime_error {
            public:
                explicit checksum_error( const std::string& what_arg ) : std::runtime_error ( what_arg ) {}
            };

            class modbus_exception : public std::runtime_error {
            public:
                std::uint8_t modbus_exception_code;
                modbus_exception( const std::string& what_arg , std::uint8_t exception_code ) :
                    std::runtime_error ( what_arg ),
                    modbus_exception_code( exception_code )
                    {}
            };

            class empty_response : public std::runtime_error {
            public:
                explicit empty_response( const std::string& what_arg ) : std::runtime_error ( what_arg ) {}
            };

            class should_never_happen : public std::runtime_error {
            public:
                explicit should_never_happen( const std::string& what_arg ) : std::runtime_error ( what_arg ) {}
            };

        } // namespace exceptions
    } // namespace modbus
}; // namespace everest

#endif
