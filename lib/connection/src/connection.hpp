#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>
#include <cstdint>

namespace everest { namespace connection {
    class Connection {
        protected:
            int connection_status;

        public:       
            Connection() : connection_status(-1) {};
            virtual ~Connection() = default;
            virtual int make_connection() = 0;
            virtual int close_connection() = 0;
            virtual int send_bytes(const std::vector<uint8_t>& bytes_to_send) = 0;
            virtual std::vector<uint8_t> receive_bytes(unsigned int number_of_bytes) = 0;
            virtual bool is_valid() = 0;
    };

 } // namespace connection
}; // namespace everest

#endif