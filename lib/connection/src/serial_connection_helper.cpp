#include <connection/serial_connection_helper.hpp>

#include <string.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <stdexcept>

int everest::connection::serial_connection_helper::openSerialDevice( const std::string device ) {

    // using namespace std::string_literals;

    int fd = open( device.c_str() , O_RDWR );

    if ( not ( fd == -1 ) )
        return fd;

    throw std::runtime_error( "Error open serial device: " + device + " Reason: " + strerror( errno ) );

}

int everest::connection::serial_connection_helper::closeSerialDevice( int serial_port_fd ) {

    return ::close( serial_port_fd );
}

void everest::connection::serial_connection_helper::getConfiguration( int serial_port_fd, termios* tty ) {

    if( tcgetattr(serial_port_fd, tty) != 0 ) {
        int myerror = errno;
        throw std::runtime_error( "Error " + std::to_string( myerror ) + " from tcgetattr: " + strerror(myerror));
    }

}

void everest::connection::serial_connection_helper::updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ ) {

    // using namespace std::string_literals;

    // we are a bit verbose with these settings...
    // BSM power meter used this as default:
    // 19200 Baud
    cfsetspeed( tty, B19200);
    // Parity
    tty->c_cflag  |= PARENB;
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

    tty->c_lflag &= ~ECHO; // Disable echo
    tty->c_lflag &= ~ECHOE; // Disable erasure
    tty->c_lflag &= ~ECHONL; // Disable new-line echo

    tty->c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty->c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty->c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

 }

void everest::connection::serial_connection_helper::updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds ) {
   // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
   // play with this... we will see if this is needed.
   // VMIN is a character count ranging from 0 to 255 characters, and VTIME is time measured in 0.1 second intervals, (0 to 25.5 seconds).
   tty->c_cc[VTIME] = timeout_deciseconds; // wait at most for timeout_deciseconds for the first char to read
   tty->c_cc[VMIN]  = 0;
}

void everest::connection::serial_connection_helper::configureDevice( int serial_port_fd, termios* tty ) {

   // Save tty settings, also checking for error
   if ( tcsetattr(serial_port_fd, TCSANOW, tty) != 0) {
       int myerror = errno;
       throw std::runtime_error("Error " + std::to_string( myerror ) + " from tcsetattr: " + strerror( myerror ));
   }
}

::size_t everest::connection::serial_connection_helper::writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count ) {

    ::size_t bytes_written_sum = 0;

    while ( bytes_written_sum < count ) {
    // write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
    // On success, the number of bytes written is returned.  On error, -1 is returned, and errno is set to indicate the error.
        ssize_t bytes_written = ::write( serial_port_fd, buffer, count );
        if ( bytes_written == -1 ) {
            // handle error
            int myerror = errno;
            throw std::runtime_error( "Error: " + std::to_string( myerror ) + " from ::write: " + strerror( myerror ));
        }
        bytes_written_sum += bytes_written;
    }
    return bytes_written_sum;
}

::ssize_t readByteFromDevice( int fd, unsigned char* charToRead ) {

    ssize_t read_result = ::read( fd, charToRead, 1 );
    if( not ( read_result == -1 ))
        return read_result;

    int myerror = errno;
    throw std::runtime_error("Error: " + std::to_string( myerror ) + " from ::read: " + strerror( myerror ));

}

::size_t everest::connection::serial_connection_helper::readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config ) {

    static_assert( std::is_unsigned<decltype(count)>::value );

    if ( count == 0 )
        return 0;

    // read at least one byte
    std::size_t index = 0;
    ::size_t bytes_read = readByteFromDevice( serial_port_fd, &buffer[ index++ ] );
    if ( bytes_read == 0 )
        return 0;

    // dont wait that long after the end of transmission as it is done at the beginning.
    updateTimeoutConfiguration( tty_config, 2 );
    configureDevice( serial_port_fd, tty_config );

    while ( bytes_read < count ) {

        auto bytes_current = readByteFromDevice( serial_port_fd, &buffer[ index++ ] );
        if ( bytes_current == 0 )
            return bytes_read;
        bytes_read += bytes_current;

    }
    return bytes_read;
}
