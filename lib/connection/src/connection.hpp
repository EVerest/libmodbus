/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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