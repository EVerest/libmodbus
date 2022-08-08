#ifndef ENDIAN_TOOLS_H_
#define ENDIAN_TOOLS_H_

#include <cstdint>

inline std::uint8_t high_byte ( std::uint16_t val ) { return ( val >> 8 ) & 0xff ; }
inline std::uint8_t low_byte  ( std::uint16_t val ) { return val & 0xff ; }
inline std::uint16_t big_endian( std::uint8_t high_byte, std::uint8_t low_byte ) { return ( high_byte << 8 ) | low_byte; }

#endif // ENDIAN_TOOLS_H_
