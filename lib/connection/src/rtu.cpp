#include <connection/connection.hpp>

#include <connection/serial_connection_helper.hpp>
#include <everest/logging.hpp>

using namespace everest::connection;
using namespace everest::connection::serial_connection_helper;

RTUConnection::RTUConnection( const RTUConnectionConfiguration& cfg ) :
    m_config( cfg )
{
    make_connection();
}

int RTUConnection::make_connection() {

    try {
        close_connection(); // is this ok?

        m_fd = openSerialDevice(m_config.m_device);
        getConfiguration( m_fd, &m_tty_config );
        updateConfiguration( &m_tty_config /*, options */ );
        updateTimeoutConfiguration( &m_tty_config, m_config.m_initial_wait_deciseconds );
        configureDevice( m_fd, &m_tty_config );
        connection_status = 1; // connection should be valid

    } catch ( const std::runtime_error& e ) {
        EVLOG_error << "Error making RTU connection: " << e.what() << std::endl;
    }

    return connection_status;
}

int RTUConnection::close_connection() {

    connection_status = -1;
    auto result = closeSerialDevice( m_fd );
    m_fd = -1;
    return result;
}

int RTUConnection::send_bytes(const std::vector<uint8_t>& bytes_to_send) {

    try {
        return writeToDevice(m_fd, bytes_to_send.data(), bytes_to_send.size());
    } catch ( const std::runtime_error& e ) {
        close_connection();
        EVLOG_error << "Error writing on RTU connection: " << e.what() << std::endl;
    }
    return -1;
}

std::vector<uint8_t> RTUConnection::receive_bytes(unsigned int number_of_bytes) {

    if ( not is_valid() ) {
        throw std::runtime_error("attempt to read on invalid rtu connection.");
    }

    std::vector<uint8_t> result(number_of_bytes);

    try {
        auto bytes_read = readFromDevice(m_fd, result.data(), number_of_bytes, &m_tty_config );
        EVLOG_debug << "read " << bytes_read << " on rtu connection." << std::endl;
    } catch ( const std::runtime_error& e ) {
        close_connection();
        EVLOG_error << "Error reading on RTU connection: " << e.what() << std::endl;
     }
    return result;

}

bool RTUConnection::is_valid() const {
    return connection_status != -1;
}
