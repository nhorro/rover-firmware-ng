#include "Protocol/protocol.hpp"

uint32_t get_current_time_ms()
{
    return std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint8_t calc_crc8(const uint8_t* data, uint8_t length)
{
    uint8_t crc = 0xff;
    size_t i, j;
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}
