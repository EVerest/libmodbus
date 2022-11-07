// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <connection/exceptions.hpp>
#include <connection/serial_connection_helper.hpp>

#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

// TODO: when SerialDevice class is mature enough, the serial_connection_helper can be moved there.
namespace everest {
namespace connection {
namespace serial_connection_helper {

int open_serial_device(const std::string& device);
int close_serial_device(int serial_port_fd);
void get_default_configuration(int serial_port_fd, termios* tty);
void set_default_configuration(termios* tty);
void update_timeout_configuration(termios* tty, unsigned int timeout_deciseconds);
void set_baudrate(termios* tty, everest::connection::SerialDeviceConfiguration::BaudRate);
void configure_device(int serial_port_fd, termios* tty);
::size_t write_to_device(int serial_port_fd, const unsigned char* const buffer, ::size_t count);
::size_t read_from_device(int serial_port_fd, unsigned char* buffer, ::size_t count, termios* tty_config,
                          unsigned int initial_timeout_deciseconds, unsigned int timeout_deciseconds);
} // namespace serial_connection_helper
} // namespace connection
} // namespace everest

bool operator==(const termios& lhs, const termios& rhs) {
    return (lhs.c_cflag == rhs.c_cflag) and (lhs.c_iflag == rhs.c_iflag) and (lhs.c_ispeed == rhs.c_ispeed) and
           (lhs.c_lflag == rhs.c_lflag) and (lhs.c_line == rhs.c_line) and (lhs.c_oflag == rhs.c_oflag) and
           (lhs.c_ospeed == rhs.c_ospeed);
}

everest::connection::SerialDeviceConfiguration::BaudrateFromIntResult
everest::connection::SerialDeviceConfiguration::baudrate_from_integer(int baudrate) {
    switch (baudrate) {
    case 1200:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_1200, true};
    case 2400:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_2400, true};
    case 4800:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_4800, true};
    case 9600:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_9600, true};
    case 19200:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_19200, true};
    case 38400:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_38400, true};
    case 57600:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_57600, true};
    case 115200:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_115200, true};
    case 230400:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_230400, true};
    default:
        return {everest::connection::SerialDeviceConfiguration::BaudRate::Baud_9600, false};
    }
}

namespace ecs = everest::connection::serial_connection_helper;

everest::connection::SerialDeviceConfiguration::SerialDeviceConfiguration(std::string device) : m_device(device) {

    int fd = ecs::open_serial_device(device);
    ecs::get_default_configuration(fd, &m_tty_config);
    ecs::close_serial_device(fd);
}

void everest::connection::SerialDevice::open() {

    close();
    m_fd = ::ecs::open_serial_device(get_serial_device_config().m_device);
    ::ecs::configure_device(m_fd, &get_serial_device_config().m_tty_config);
}

void everest::connection::SerialDevice::close() {

    ::ecs::close_serial_device(m_fd);
    m_fd = -1;
}

everest::connection::SerialDeviceConfiguration& everest::connection::SerialDevice::get_serial_device_config() {

    return m_serial_device_configuration;
}

::size_t everest::connection::SerialDevice::write(const unsigned char* const buffer, ::size_t count) {

    return ::ecs::write_to_device(m_fd, buffer, count);
}

void everest::connection::SerialDevice::drain() {
    tcdrain(m_fd);
}

::size_t everest::connection::SerialDevice::read(unsigned char* buffer, ::size_t count) {

    return ::ecs::read_from_device(m_fd, buffer, count, &get_serial_device_config().m_tty_config,
                                   get_serial_device_config().initial_read_timeout_deciseconds,
                                   get_serial_device_config().default_read_timeout_deciseconds);
}

::size_t everest::connection::SerialDeviceLogToStream::write(const unsigned char* const buffer, ::size_t count) {

    (*m_stream) << get_serial_device_config().m_device << " write: \n";
    (*m_stream) << std::hex;
    for (::size_t index = 0; index < count; ++index)
        (*m_stream) << (unsigned)buffer[index] << " ";
    (*m_stream) << "\n\n" << std::flush;
    return SerialDevice::write(buffer, count);
}

::size_t everest::connection::SerialDeviceLogToStream::read(unsigned char* buffer, ::size_t count) {

    auto delay = get_serial_device_config().m_tty_config.c_cc[VTIME];
    ::size_t bytes_read = SerialDevice::read(buffer, count);
    (*m_stream) << get_serial_device_config().m_device << " read: \n with delay : " << (int)delay << "\n";
    (*m_stream) << std::hex;
    for (::size_t index = 0; index < bytes_read; ++index)
        (*m_stream) << (unsigned)buffer[index] << " ";
    (*m_stream) << "\n\n" << std::flush;
    return bytes_read;
}

int ecs::open_serial_device(const std::string& device) {

    int fd = open(device.c_str(), O_RDWR);

    if (not(fd == -1))
        return fd;

    throw everest::connection::exceptions::tty::tty_error(
        "Error open serial device: " + device + " Reason: " + strerror(errno), errno);
}

int ecs::close_serial_device(int serial_port_fd) {

    return ::close(serial_port_fd);
}

void ecs::get_default_configuration(int serial_port_fd, termios* tty) {

    if (tcgetattr(serial_port_fd, tty) != 0) {
        int myerror = errno;
        throw everest::connection::exceptions::tty::tty_error(
            "Error " + std::to_string(myerror) + " from tcgetattr: " + strerror(myerror), myerror);
    }
}

void ecs::set_default_configuration(termios* tty) {

    // we are a bit verbose with these settings...
    // BSM power meter used this as default:
    // 19200 Baud
    cfsetspeed(tty, B19200);
    // Parity
    tty->c_cflag |= PARENB;
    // 8 Data bits
    tty->c_cflag &= ~CSIZE;
    tty->c_cflag |= CS8;
    // 1 Stop bit
    tty->c_cflag &= ~CSTOPB;
    // no CRTSCTS (?)
    tty->c_cflag &= ~CRTSCTS;

    // turn off modem specific stuff...
    tty->c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    // disable canonical mode (in canonical mode, data is processed after newline).
    tty->c_lflag &= ~ICANON;

    tty->c_lflag &= ~ECHO;    // Disable echo
    tty->c_lflag &= ~ECHOE;   // Disable erasure
    tty->c_lflag &= ~ECHONL;  // Disable new-line echo
    tty->c_lflag &= ~ECHOCTL; //
    tty->c_lflag &= ~ECHOKE;
    tty->c_lflag &= ~ECHOK;

    tty->c_lflag &= ~IEXTEN;

    tty->c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL |
                      IXON); // Disable any special handling of received bytes

    tty->c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty->c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
}

void ecs::update_timeout_configuration(termios* tty, unsigned int timeout_deciseconds) {
    // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
    // play with this... we will see if this is needed.
    // VMIN is a character count ranging from 0 to 255 characters, and VTIME is time measured in 0.1 second intervals,
    // (0 to 25.5 seconds).
    tty->c_cc[VTIME] = timeout_deciseconds; // wait at most for timeout_deciseconds for the first char to read
    tty->c_cc[VMIN] = 0;
}

void ecs::set_baudrate(termios* tty, everest::connection::SerialDeviceConfiguration::BaudRate baudrate) {
    cfsetspeed(tty,
               static_cast<std::underlying_type_t<everest::connection::SerialDeviceConfiguration::BaudRate>>(baudrate));
}

void ecs::configure_device(int serial_port_fd, termios* tty) {

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port_fd, TCSANOW, tty) != 0) {
        int myerror = errno;
        throw everest::connection::exceptions::tty::tty_error(
            "Error " + std::to_string(myerror) + " from tcsetattr: " + strerror(myerror), myerror);
    }
}

::size_t ecs::write_to_device(int serial_port_fd, const unsigned char* const buffer, ::size_t count) {

    ::size_t bytes_written_sum = 0;

    while (bytes_written_sum < count) {
        // write() writes up to count bytes from the buffer starting at buf to the file referred to by the file
        // descriptor fd. On success, the number of bytes written is returned.  On error, -1 is returned, and errno is
        // set to indicate the error.
        ssize_t bytes_written = ::write(serial_port_fd, buffer, count);
        if (bytes_written == -1) {
            // handle error
            int myerror = errno;
            throw everest::connection::exceptions::tty::tty_error(
                "Error: " + std::to_string(myerror) + " from ::write: " + strerror(myerror), myerror);
        }
        bytes_written_sum += bytes_written;
    }

    tcdrain(serial_port_fd);
    return bytes_written_sum;
}

::ssize_t readByteFromDevice(int fd, unsigned char* charToRead) {

    ssize_t read_result = ::read(fd, charToRead, 1);
    if (not(read_result == -1))
        return read_result;

    int myerror = errno;
    throw everest::connection::exceptions::tty::tty_error(
        "Error: " + std::to_string(myerror) + " from ::read: " + strerror(myerror), myerror);
}

::size_t ecs::read_from_device(int serial_port_fd, unsigned char* buffer, ::size_t count, termios* tty_config,
                               unsigned int initial_timeout_deciseconds, unsigned int timeout_deciseconds) {

    static_assert(std::is_unsigned<decltype(count)>::value, "need an unsigned type here. ");
    update_timeout_configuration(tty_config, initial_timeout_deciseconds);
    configure_device(serial_port_fd, tty_config);

    if (count == 0)
        return 0;

    // read at least one byte
    std::size_t index = 0;
    ::size_t bytes_read = readByteFromDevice(serial_port_fd, &buffer[index++]);
    if (bytes_read == 0)
        return 0;

    // dont wait that long after the end of transmission as it is done at the beginning.
    update_timeout_configuration(tty_config, timeout_deciseconds);
    configure_device(serial_port_fd, tty_config);

    while (bytes_read < count) {

        auto bytes_current = readByteFromDevice(serial_port_fd, &buffer[index++]);
        if (bytes_current == 0)
            return bytes_read;
        bytes_read += bytes_current;
    }
    return bytes_read;
}

using namespace std::string_literals;

void everest::connection::SerialDeviceConfiguration::SerialDeviceConfiguration::get_current_config() {

    int serial_port = open(m_device.c_str(), O_RDWR);
    if (tcgetattr(serial_port, &m_tty_config) != 0) {
        throw everest::connection::exceptions::tty::tty_error(""s + __PRETTY_FUNCTION__ + "Error : " + strerror(errno),
                                                              errno);
    }
    close(serial_port);
}

everest::connection::SerialDeviceConfiguration&
everest::connection::SerialDeviceConfiguration::set_baud_rate(BaudRate baudrate) {
    cfsetspeed(&m_tty_config,
               static_cast<std::underlying_type_t<everest::connection::SerialDeviceConfiguration::BaudRate>>(baudrate));
    return *this;
}

everest::connection::SerialDeviceConfiguration&
everest::connection::SerialDeviceConfiguration::set_data_bits(DataBits data_bits) {
    m_tty_config.c_cflag &= ~CSIZE; // clear bits
    m_tty_config.c_cflag |=
        static_cast<std::underlying_type_t<everest::connection::SerialDeviceConfiguration::DataBits>>(data_bits);
    return *this;
}

everest::connection::SerialDeviceConfiguration&
everest::connection::SerialDeviceConfiguration::set_stop_bits(StopBits stop_bits) {

    switch (stop_bits) {
    case StopBits::One:
        m_tty_config.c_cflag &= ~CSTOPB;
        break;
    case StopBits::Two:
        m_tty_config.c_cflag |= CSTOPB;
        break;
    }
    return *this;
}

everest::connection::SerialDeviceConfiguration&
everest::connection::SerialDeviceConfiguration::set_parity(Parity parity) {

    switch (parity) {
    case Parity::None:
        m_tty_config.c_cflag &= ~PARENB;
        m_tty_config.c_cflag &= ~PARODD;
        break;
    case Parity::Even:
        m_tty_config.c_cflag |= PARENB;
        m_tty_config.c_cflag &= ~PARODD;
        break;
    case Parity::Odd:
        m_tty_config.c_cflag |= PARENB;
        m_tty_config.c_cflag |= PARODD;
        break;
    }
    return *this;
}

everest::connection::SerialDeviceConfiguration&
everest::connection::SerialDeviceConfiguration::set_sensible_defaults() {

    ecs::set_default_configuration(&m_tty_config);
    ecs::update_timeout_configuration(&m_tty_config, initial_read_timeout_deciseconds);
    return *this;
}
