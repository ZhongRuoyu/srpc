#ifndef SRPC_PROTOCOL_INTEGERS_H_
#define SRPC_PROTOCOL_INTEGERS_H_

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "srpc/utils/memory.h"

namespace srpc {

// NOLINTBEGIN(readability-identifier-naming)

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

// NOLINTEND(readability-identifier-naming)

template <typename T>
[[nodiscard]] inline constexpr T ReverseBytes(T val) {
  if constexpr (sizeof(T) == 8) {
    val = (std::make_unsigned_t<T>(val) & 0x00ff00ff00ff00ff) << 8 |
          (std::make_unsigned_t<T>(val) & 0xff00ff00ff00ff00) >> 8;
    return ((std::make_unsigned_t<T>(val) & 0x000000000000ffff) << 48) |
           ((std::make_unsigned_t<T>(val) & 0x00000000ffff0000) << 16) |
           ((std::make_unsigned_t<T>(val) & 0x0000ffff00000000) >> 16) |
           ((std::make_unsigned_t<T>(val) & 0xffff000000000000) >> 48);
  }
  if constexpr (sizeof(T) == 4) {
    return ((std::make_unsigned_t<T>(val) & 0x000000ff) << 24) |
           ((std::make_unsigned_t<T>(val) & 0x0000ff00) << 8) |
           ((std::make_unsigned_t<T>(val) & 0x00ff0000) >> 8) |
           ((std::make_unsigned_t<T>(val) & 0xff000000) >> 24);
  }
  if constexpr (sizeof(T) == 2) {
    return ((std::make_unsigned_t<T>(val) & 0x00ff) << 8) |
           ((std::make_unsigned_t<T>(val) & 0xff00) >> 8);
  }
  if constexpr (sizeof(T) == 1) {
    return val;
  }
  assert(false);
}

template <typename T>
[[nodiscard]] inline constexpr std::array<std::byte, sizeof(T)> Serialize(
    T val) {
  if constexpr (std::endian::native == std::endian::big) {
    val = ReverseBytes(val);
  }
  std::array<std::byte, sizeof(T)> data;
  Memcpy(data.data(), (std::byte *)(&val), sizeof(T));
  return data;
}

template <typename T>
[[nodiscard]] inline constexpr T Deserialize(
    std::array<std::byte, sizeof(T)> data) {
  T val;
  Memcpy((std::byte *)(&val), data.data(), sizeof(T));
  if constexpr (std::endian::native == std::endian::big) {
    val = ReverseBytes(val);
  }
  return val;
}

}  // namespace srpc

#endif  // SRPC_PROTOCOL_INTEGERS_H_
