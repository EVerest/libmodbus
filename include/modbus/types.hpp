#ifndef TYPES_H_
#define TYPES_H_

#include <vector>

namespace everest {
namespace modbus {

using DataVectorUint16 = std::vector<std::uint16_t>;
using DataVectorUint8 = std::vector<std::uint8_t>;

enum struct ByteOrder {
    BigEndian,
    LittleEndian
};

} // namespace modbus
} // namespace everest

#endif // TYPES_H_
