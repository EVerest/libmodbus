#ifndef MODBUS_TRANSPORT_H_
#define MODBUS_TRANSPORT_H_

#include <string>

#include "modbus_messages.hpp"
#include "serial_device.hpp"

namespace ModbusTransport {

    struct RTUConfig {

        std::string device { "/dev/ttyUSB0" };
        std::uint8_t unit_id;
        // std::string m_baud = "19200"; //hardcoded for now
        unsigned int m_initial_wait_deciseconds = 50; // wait 5 secs for response
    };


    class AbstractModbusTransport {

    public:

        // max number of bytes to transfer (another name for adu)
        virtual std::size_t max_message_size() const = 0;

        // max number of bytes to transfer (application data unit), payload and protocol bytes.
        virtual std::size_t max_adu_size()     const = 0;

        // max payload size (function code and function specific data).
        virtual std::size_t max_pdu_size()     const = 0;

        virtual void send( const ModbusMessages::AbstractModbusQuery&  ) = 0;
        virtual void read( ModbusMessages::ModbusResponse&  ) = 0;
        virtual void verify_response( ModbusMessages::ModbusResponse& ) const = 0;

    };


    struct RTUTransaction {
        std::uint8_t m_unit_id;
        std::uint8_t m_function_code;
    };


    class RTU : public AbstractModbusTransport {

    private:

        RTUConfig m_config {};

        SerialDevice m_serial_device;

        RTUTransaction m_transaction;

    public:

        RTU( RTUConfig config ) :
            m_config( config )
            {}

        // max number of bytes to transfer (another name for adu)
        std::size_t max_message_size() const override { return max_adu_size(); }

        // max number of bytes to transfer.
        std::size_t max_adu_size() const override { return ModbusMessages::RTU::MAX_ADU; }

        // max payload size (function code and function specific data).
        std::size_t max_pdu_size() const override { return ModbusMessages::RTU::MAX_PDU; }

        bool connected() const { return not ( m_serial_device.fd == -1 ); }

        void connect();

        void close();

        void verify_exception_code( ModbusMessages::ModbusResponse& response ) const ;

        // rtu implementaion
        void verify_response( ModbusMessages::ModbusResponse& response ) const override ;

        // this is the rtu transport
        void verify_transaction( ModbusMessages::ModbusResponse& response ) ;

        void send( const ModbusMessages::AbstractModbusQuery& query ) override ;

        void read( ModbusMessages::ModbusResponse& response ) override ;

        void operator << ( const ModbusMessages::AbstractModbusQuery& query ) ;

        void operator >> ( ModbusMessages::ModbusResponse& response ) ;
    };
}


#endif // MODBUS_TRANSPORT_H_
