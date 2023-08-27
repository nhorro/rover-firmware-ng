#pragma once

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define FIX_ENDIANNESS_64(x) __builtin_bswap64(x)
    #define FIX_ENDIANNESS_32(x) __builtin_bswap32(x)
    #define FIX_ENDIANNESS_16(x) __builtin_bswap16(x)    
#else
    #define FIX_ENDIANNESS_32(x) x
#endif

#include <chrono>

constexpr size_t HEADER_SIZE = (2+1); /* SYNC + LENGTH */
constexpr size_t PAYLOAD_BUFFER_SIZE =  256;
constexpr size_t TRAILER_SIZE = (1+1); /* CRC8 + TERMINATOR */
constexpr size_t PACKET_TIMEOUT_IN_MS =1000;
constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + PAYLOAD_BUFFER_SIZE + TRAILER_SIZE;

constexpr char PACKET_SYNC_0_CHAR = '@';
constexpr char PACKET_SYNC_1_CHAR = '<';
constexpr char PACKET_TERMINATOR_CHAR = '\n';

/**
 * @brief 
 * 
 * @tparam T 
 * @param s 
 * @param bytes 
 */
template <typename T> void to_bytes(const T& s, void* bytes);

/**
 * @brief 
 * 
 * @tparam T 
 * @param s 
 * @param bytes 
 */
template <typename T> void from_bytes(T& s, const void* bytes);


uint32_t get_current_time_ms();

uint8_t calc_crc8(const uint8_t* data, uint8_t length);
