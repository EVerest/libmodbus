// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>

using namespace everest::modbus;

ModbusRTUClient::ModbusRTUClient(connection::Connection& conn_) : ModbusClient(conn_) {
    // TBD;
}

const std::vector<uint8_t> ModbusRTUClient::full_message_from_body(const std::vector<uint8_t>& body, uint16_t message_length, uint8_t unit_id) const {
    // TBD

    // Modbus RTU frame format

    // This format is primarily used on asynchronous serial data lines like RS-485/EIA-485. Its name refers to a remote terminal unit.
    // Name     Length (bits)   Function
    // Start    3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition)
    // Address      8   Station address
    // Function     8   Indicates the function code e.g. "read coils"
    // Data     n × 8   Data + length will be filled depending on the message type
    // CRC      16      Cyclic redundancy check
    // End      3.5 x 8     At least 3+1⁄2 character times (28 bits) of silence (mark condition) between frames


    // TODO: Seems that the connection object has to implement the framing...?

    // RTU Framing
    // In RTU mode, messages start with a silent interval of at least 3.5 character times.
    // This is most easily implemented as a multiple of character times at the baud rate
    // that is being used on the network (shown as T1–T2–T3–T4 in the figure below).
    // The first field then transmitted is the device address.
    // The allowable characters transmitted for all fields are hexadecimal 0–9, A–F.
    // Networked devices monitor the network bus continuously, including during the
    // ‘silent’ intervals. When the first field (the address field) is received, each device
    // decodes it to find out if it is the addressed device.
    // Following the last transmitted character, a similar interval of at least 3.5 character
    // times marks the end of the message. A new message can begin after this interval.
    // The entire message frame must be transmitted as a continuous stream. If a silent
    // interval of more than 1.5 character times occurs before completion of the frame,
    // the receiving device flushes the incomplete message and assumes that the next
    // byte will be the address field of a new message.
    // Similarly, if a new message begins earlier than 3.5 character times following a
    // previous message, the receiving device will consider it a continuation of the
    // previous message. This will set an error, as the value in the final CRC field will not
    // be valid for the combined messages.

    return body;
}
