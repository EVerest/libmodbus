#include <string>
#include <cstdint>
#include <vector>

#include "../src/connection.hpp"

namespace everest { namespace connection {
    class TCPConnection : public Connection {
        private:
            int port;
            std::string address;
            int socket_fd;

        public:
            TCPConnection(const std::string& address_, int port_);
            ~TCPConnection();
            int make_connection();
            int send_bytes(const std::vector<uint8_t>& bytes_to_send);
            std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes);
            int close_connection();
            bool is_valid();
    };

 } // namespace connection
}; // namespace everest