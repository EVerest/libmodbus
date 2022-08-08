#ifndef CRC16_H_
#define CRC16_H_

#include <cstdint>

std::uint16_t calcCRC_16_ANSI( const std::uint8_t* payload, std::size_t payload_length );

#endif // CRC16_H_
