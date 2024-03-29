// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#pragma once

#include <stdexcept>

namespace everest {

namespace connection {

namespace exceptions {

class connection_error : public std::runtime_error {
public:
    connection_error(const std::string& what_arg) : std::runtime_error(what_arg) {
    }
};

class communication_error : public std::runtime_error {
public:
    communication_error(const std::string& what_arg) : std::runtime_error(what_arg) {
    }
};

namespace tcp {
class tcp_connection_error : public connection_error {
public:
    tcp_connection_error(const std::string& what_arg) : connection_error(what_arg) {
    }
};
}; // namespace tcp

namespace udp {
class udp_socket_error : public connection_error {
public:
    udp_socket_error(const std::string& what_arg) : connection_error(what_arg) {
    }
};
}; // namespace udp

namespace tty {
class tty_error : public connection_error {

public:
    int error_number;
    tty_error(const std::string& what_arg, int tty_error_number) :
        connection_error(what_arg), error_number(tty_error_number) {
    }
};
} // namespace tty

} // namespace exceptions
} // namespace connection
}; // namespace everest
