#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

namespace everest { namespace connection { namespace utils {
    std::string get_bytes_hex_string(const std::vector<uint8_t>& bytes);
} } };

#endif