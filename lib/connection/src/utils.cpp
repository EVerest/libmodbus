#include <connection/utils.hpp>

using namespace everest::connection;

std::string utils::get_bytes_hex_string(const std::vector<uint8_t>& bytes) {
    std::stringstream buffer;
    for (auto it = bytes.begin() ; it != bytes.end() ; it++) {
        buffer << std::hex << (int) *it << " ";
    }
    return buffer.str();
}