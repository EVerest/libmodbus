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
#include <vector>
#include <memory>

#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

int main() {
    everest::connection::TCPConnection conn("127.0.0.1", 502);
    if (!conn.is_valid()) return 0;
    everest::modbus::ModbusTCPClient client = everest::modbus::ModbusTCPClient(conn);
    std::vector<uint8_t> v = client.read_holding_register(1, 40000, 1);
    everest::modbus::utils::print_message_hex(v);
}