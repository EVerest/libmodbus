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
#ifndef CONNECTION_EXCEPTIONS_H
#define CONNECTION_EXCEPTIONS_H

#include <stdexcept>

namespace everest { namespace connection { namespace exceptions {

    class connection_error : public std::runtime_error {
        public:
            connection_error(const std::string& what_arg): std::runtime_error(what_arg) {}
    };
    class communication_error : public std::runtime_error {
        public:
            communication_error(const std::string& what_arg): std::runtime_error(what_arg) {}
    };

    namespace tcp {
        class tcp_connection_error : public connection_error {
            public:
                tcp_connection_error(const std::string& what_arg) : connection_error(what_arg) {}
        };
    };

  } // namespace exceptions
 }  // namespace connection
};  // namespace everest

#endif