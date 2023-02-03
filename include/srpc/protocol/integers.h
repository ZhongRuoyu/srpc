#ifndef SRPC_PROTOCOL_INTEGERS_H_
#define SRPC_PROTOCOL_INTEGERS_H_

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "srpc/protocol/serialization.h"

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

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
[[nodiscard]] inline constexpr T ReverseBytes(T val) {
  using U = std::make_unsigned_t<T>;
  if constexpr (sizeof(T) == 8) {
    val =
        (U(val) & 0x00ff00ff00ff00ff) << 8 | (U(val) & 0xff00ff00ff00ff00) >> 8;
    return ((U(val) & 0x000000000000ffff) << 48) |
           ((U(val) & 0x00000000ffff0000) << 16) |
           ((U(val) & 0x0000ffff00000000) >> 16) |
           ((U(val) & 0xffff000000000000) >> 48);
  }
  if constexpr (sizeof(T) == 4) {
    return ((U(val) & 0x000000ff) << 24) | ((U(val) & 0x0000ff00) << 8) |
           ((U(val) & 0x00ff0000) >> 8) | ((U(val) & 0xff000000) >> 24);
  }
  if constexpr (sizeof(T) == 2) {
    return ((U(val) & 0x00ff) << 8) | ((U(val) & 0xff00) >> 8);
  }
  if constexpr (sizeof(T) == 1) {
    return val;
  }
  assert(false);
}

template <>
struct Marshal<i8> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(i8)> operator()(
      i8 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {std::byte(val)};
  }
};

template <>
struct Marshal<i16> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(i16)> operator()(
      i16 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),
        std::byte((val >> 8) & 0xff),
    };
  }
};

template <>
struct Marshal<i32> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(i32)> operator()(
      i32 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),
        std::byte((val >> 8) & 0xff),
        std::byte((val >> 16) & 0xff),
        std::byte((val >> 24) & 0xff),
    };
  }
};

template <>
struct Marshal<i64> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(i64)> operator()(
      i64 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),         std::byte((val >> 8) & 0xff),
        std::byte((val >> 16) & 0xff), std::byte((val >> 24) & 0xff),
        std::byte((val >> 32) & 0xff), std::byte((val >> 40) & 0xff),
        std::byte((val >> 48) & 0xff), std::byte((val >> 56) & 0xff),
    };
  }
};

template <>
struct Marshal<u8> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(u8)> operator()(
      u8 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {std::byte(val)};
  }
};

template <>
struct Marshal<u16> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(u16)> operator()(
      u16 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),
        std::byte((val >> 8) & 0xff),
    };
  }
};

template <>
struct Marshal<u32> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(u32)> operator()(
      u32 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),
        std::byte((val >> 8) & 0xff),
        std::byte((val >> 16) & 0xff),
        std::byte((val >> 24) & 0xff),
    };
  }
};

template <>
struct Marshal<u64> {
  [[nodiscard]] constexpr std::array<std::byte, sizeof(u64)> operator()(
      u64 val) const {
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return {
        std::byte(val & 0xff),         std::byte((val >> 8) & 0xff),
        std::byte((val >> 16) & 0xff), std::byte((val >> 24) & 0xff),
        std::byte((val >> 32) & 0xff), std::byte((val >> 40) & 0xff),
        std::byte((val >> 48) & 0xff), std::byte((val >> 56) & 0xff),
    };
  }
};

template <>
struct Unmarshal<i8> {
  [[nodiscard]] constexpr i8 operator()(
      std::array<std::byte, sizeof(i8)> data) const {
    i8 val = i8(data[0]);
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<i16> {
  [[nodiscard]] constexpr i16 operator()(
      std::array<std::byte, sizeof(i16)> data) const {
    i16 val = i16(i16(data[0]) | (i16(data[1]) << 8));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<i32> {
  [[nodiscard]] constexpr i32 operator()(
      std::array<std::byte, sizeof(i32)> data) const {
    i32 val = i32(i32(data[0]) | (i32(data[1]) << 8) | (i32(data[2]) << 16) |
                  (i32(data[3]) << 24));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<i64> {
  [[nodiscard]] constexpr i64 operator()(
      std::array<std::byte, sizeof(i64)> data) const {
    i64 val =
        i64(i64(data[0]) | (i64(data[1]) << 8) | (i64(data[2]) << 16) |
            (i64(data[3]) << 24) | (i64(data[4]) << 32) | (i64(data[5]) << 40) |
            (i64(data[6]) << 48) | (i64(data[7]) << 56));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<u8> {
  [[nodiscard]] constexpr u8 operator()(
      std::array<std::byte, sizeof(u8)> data) const {
    u8 val = u8(data[0]);
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<u16> {
  [[nodiscard]] constexpr u16 operator()(
      std::array<std::byte, sizeof(u16)> data) const {
    u16 val = u16(u16(data[0]) | (u16(data[1]) << 8));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<u32> {
  [[nodiscard]] constexpr u32 operator()(
      std::array<std::byte, sizeof(u32)> data) const {
    u32 val = u32(u32(data[0]) | (u32(data[1]) << 8) | (u32(data[2]) << 16) |
                  (u32(data[3]) << 24));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

template <>
struct Unmarshal<u64> {
  [[nodiscard]] constexpr u64 operator()(
      std::array<std::byte, sizeof(u64)> data) const {
    u64 val =
        u64(u64(data[0]) | (u64(data[1]) << 8) | (u64(data[2]) << 16) |
            (u64(data[3]) << 24) | (u64(data[4]) << 32) | (u64(data[5]) << 40) |
            (u64(data[6]) << 48) | (u64(data[7]) << 56));
    if constexpr (std::endian::native != std::endian::little) {
      val = ReverseBytes(val);
    }
    return val;
  }
};

}  // namespace srpc

#endif  // SRPC_PROTOCOL_INTEGERS_H_
