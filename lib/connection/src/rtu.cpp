// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <connection/connection.hpp>
#include <connection/serial_connection_helper.hpp>
#include <everest/logging.hpp>

using namespace everest::connection;

RTUConnection::RTUConnection(::everest::connection::SerialDevice& serialdevice) : m_serial_device(serialdevice) {
    make_connection();
}

int RTUConnection::make_connection() {

    try {
        close_connection(); // is this ok?

        // serial device will be opened configured by the config object of m_serial_device
        m_serial_device.open();

        connection_status = 1; // connection should be valid

    } catch (const std::runtime_error& e) {
        EVLOG_error << "Error making RTU connection: " << e.what() << std::endl;
        throw;
    }

    return connection_status;
}

int RTUConnection::close_connection() {

    connection_status = -1;
    m_serial_device.close();
    return 0;
}

int RTUConnection::send_bytes(const std::vector<uint8_t>& bytes_to_send) {

    try {
        auto bytes_written = m_serial_device.write(bytes_to_send.data(), bytes_to_send.size());
        return bytes_written;
    } catch (const std::runtime_error& e) {
        close_connection();
        EVLOG_error << "Error writing on RTU connection: " << e.what() << std::endl;
        throw;
    }
    return -1;
}

std::vector<uint8_t> RTUConnection::receive_bytes(unsigned int number_of_bytes) {

    if (not is_valid()) {
        throw std::runtime_error("attempt to read on invalid rtu connection.");
    }

    std::vector<uint8_t> result(number_of_bytes);

    try {
        auto bytes_read = m_serial_device.read(result.data(), number_of_bytes);
        result.resize(bytes_read);
    } catch (const std::runtime_error& e) {
        close_connection();
        EVLOG_error << "Error reading on RTU connection: " << e.what() << std::endl;
        throw;
    }
    return result;
}

bool RTUConnection::is_valid() const {
    return connection_status != -1;
}
