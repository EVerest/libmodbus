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
#include <iostream>
#include <string>
#include <memory>
#include <boost/log/trivial.hpp>
#include <sstream>

#include <connection/tcp.hpp>
#include <connection/utils.hpp>

// simple example
int main() {
    std::unique_ptr<everest::connection::TCPConnection> tcp_conn = std::make_unique<everest::connection::TCPConnection>("127.0.0.1", 502);
    tcp_conn->make_connection();
    std::vector<uint8_t> bytes({0x3F, 0x67, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x9C, 0x41, 0x00, 0x01});
    tcp_conn->send_bytes(bytes);
    std::vector<uint8_t> received_bytes = tcp_conn->receive_bytes(40);
    return 0;
}