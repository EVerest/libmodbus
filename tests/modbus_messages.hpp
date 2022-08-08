#ifndef MODBUS_MESSAGES_H_
#define MODBUS_MESSAGES_H_

#include <cstdint>
#include <vector>
#include <optional>

namespace ModbusMessages {

    using DataVector = std::vector<std::uint8_t>;


    struct ResponseData {
        DataVector data;
        DataVector::const_iterator payload_begin { data.begin() };
        DataVector::const_iterator payload_end { data.end() };
    };


    namespace RTU {
        constexpr uint16_t MAX_ADU = 256;
        constexpr uint16_t MAX_PDU = 253;
    }


    class AbstractModbusQuery {

    public:

        virtual bool is_ready() const = 0;
        virtual std::size_t size() const = 0;
        virtual void append_query_to( DataVector& ) const = 0;

    };


    class ReadHoldingRegisterQuery : public AbstractModbusQuery {

    private:

        std::optional<std::uint16_t> m_register_address;
        std::optional<std::uint8_t> m_number_of_register_to_read;

    public:

        ReadHoldingRegisterQuery& register_address_unit16( std::uint16_t ) ;

        ReadHoldingRegisterQuery& number_of_register_to_read( std::uint8_t number_of_register_to_read) ;

        std::size_t size() const override ;

        bool is_ready() const override ;

        void append_query_to( DataVector& outgoing ) const override ;
    };


    class ModbusResponse {

    protected:

        ResponseData m_data {};

    public:

        bool empty() const ;
        std::size_t size_raw()      const;
        std::size_t size_payload () const;

        ResponseData& get_response_data_container() ;

        DataVector response_data() const ;

        static inline bool is_exception_code( std::uint8_t code ) { return not ( ( code & 0x80 ) == 0 ); }
        static inline bool function_code_without_exception_flag( std::uint8_t code ) { return code & 0x7f; }

        std::uint8_t function_code() const ;

    };


    class ReadHoldingRegisterResponse : public ModbusResponse {

    public:

    };


}


#endif // MODBUS_MESSAGES_H_
