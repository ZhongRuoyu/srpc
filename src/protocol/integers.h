#ifndef SRPC_PROTOCOL_INTEGERS_H_
#define SRPC_PROTOCOL_INTEGERS_H_

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace srpc {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

template <typename T>
inline constexpr T Byteswap(T val) {
  if constexpr (sizeof(T) == 8) {
    return ((std::make_unsigned_t<T>(val) & 0xff00000000000000) >> 56) |
           ((std::make_unsigned_t<T>(val) & 0x00ff000000000000) >> 40) |
           ((std::make_unsigned_t<T>(val) & 0x0000ff0000000000) >> 24) |
           ((std::make_unsigned_t<T>(val) & 0x000000ff00000000) >> 8) |
           ((std::make_unsigned_t<T>(val) & 0x00000000ff000000) << 8) |
           ((std::make_unsigned_t<T>(val) & 0x0000000000ff0000) << 24) |
           ((std::make_unsigned_t<T>(val) & 0x000000000000ff00) << 40) |
           ((std::make_unsigned_t<T>(val) & 0x00000000000000ff) << 56);
  }
  if constexpr (sizeof(T) == 4) {
    return ((std::make_unsigned_t<T>(val) & 0xff000000) >> 24) |
           ((std::make_unsigned_t<T>(val) & 0x00ff0000) >> 8) |
           ((std::make_unsigned_t<T>(val) & 0x0000ff00) << 8) |
           ((std::make_unsigned_t<T>(val) & 0x000000ff) << 24);
  }
  if constexpr (sizeof(T) == 2) {
    return ((std::make_unsigned_t<T>(val) & 0xff00) >> 8) |
           ((std::make_unsigned_t<T>(val) & 0x00ff) << 8);
  }
  if constexpr (sizeof(T) == 1) {
    return val;
  }
  assert(false);
}

template <typename T>
inline constexpr std::array<std::byte, sizeof(T)> Serialize(T val) {
  if constexpr (std::endian::native == std::endian::big) {
    val = Byteswap(val);
  }
  std::array<std::byte, sizeof(T)> data;
  std::memcpy(data.data(), &val, sizeof(T));
  return data;
}

template <typename T>
inline constexpr T Deserialize(std::array<std::byte, sizeof(T)> data) {
  T val;
  std::memcpy(&val, data.data(), sizeof(T));
  if constexpr (std::endian::native == std::endian::big) {
    val = Byteswap(val);
  }
  return val;
}

}  // namespace srpc

#endif  // SRPC_PROTOCOL_INTEGERS_H_
