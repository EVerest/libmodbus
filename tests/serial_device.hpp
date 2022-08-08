#ifndef SERIAL_DEVICE_H_
#define SERIAL_DEVICE_H_

#include <termios.h>

#include <string>

class SerialDevice {

public:

    termios tty_config {};
    int     fd = -1 ;

    virtual int openSerialDevice( const std::string device );
    virtual int closeSerialDevice( int serial_port_fd );
    virtual void getConfiguration( int serial_port_fd, termios* tty );
    virtual void updateConfiguration( termios* tty /*, const SerialDeviceOptions& */ );
    virtual void updateTimeoutConfiguration( termios* tty , unsigned int timeout_deciseconds );
    virtual void configureDevice( int serial_port_fd, termios* tty );
    virtual ::size_t writeToDevice( int serial_port_fd, const unsigned char* const buffer, ::size_t count );
    virtual ::size_t readFromDevice( int serial_port_fd, unsigned char* buffer, ::size_t count , termios* tty_config );

};

#endif // SERIAL_DEVICE_H_
