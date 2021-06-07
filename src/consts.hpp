#ifndef MODBUS_CONSTS_H
#define MODBUS_CONSTS_H

#include <cstdint>

namespace everest { namespace modbus { namespace consts {

    // General MODBUS constants
    const uint16_t READ_HOLDING_REGISTER_MESSAGE_LENGTH = 6;
    const uint8_t READ_HOLDING_REGISTER_FUNCTION_CODE = 3;

    // MODBUS/TCP specific constants
    namespace tcp {
        const uint16_t PROTOCOL_ID = 0;
        const uint8_t MBAP_HEADER_LENGTH = 7;
        const uint16_t DEFAULT_PORT = 502;
        const uint16_t MAX_MESSAGE_SIZE = 256;
    }

  } // namespace consts
 }  // namespace modbus
};  // namespace everest

#endif