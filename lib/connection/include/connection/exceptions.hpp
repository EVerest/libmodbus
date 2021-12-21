// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef CONNECTION_EXCEPTIONS_H
#define CONNECTION_EXCEPTIONS_H

#include <stdexcept>

namespace everest { namespace connection { namespace exceptions {

    class connection_error : public std::runtime_error {
        public:
            connection_error(const std::string& what_arg): std::runtime_error(what_arg) {}
    };
    class communication_error : public std::runtime_error {
        public:
            communication_error(const std::string& what_arg): std::runtime_error(what_arg) {}
    };

    namespace tcp {
        class tcp_connection_error : public connection_error {
            public:
                tcp_connection_error(const std::string& what_arg) : connection_error(what_arg) {}
        };
    };

  } // namespace exceptions
 }  // namespace connection
};  // namespace everest

#endif