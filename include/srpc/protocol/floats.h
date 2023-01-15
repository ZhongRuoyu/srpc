#ifndef SRPC_PROTOCOL_FLOATS_H_
#define SRPC_PROTOCOL_FLOATS_H_

#include <array>
#include <cstddef>
#include <limits>

#include "srpc/protocol/integers.h"

namespace srpc {

static_assert(std::numeric_limits<float>::is_iec559);
using f32 = float;

static_assert(std::numeric_limits<double>::is_iec559);
using f64 = double;

template <>
[[nodiscard]] inline constexpr std::array<std::byte, sizeof(f32)> Serialize(
    f32 val) {
  union {
    f32 f;
    i32 i;
  } u;
  u.f = val;
  return Serialize(u.i);
}

template <>
[[nodiscard]] inline constexpr std::array<std::byte, sizeof(f64)> Serialize(
    f64 val) {
  union {
    f64 f;
    i64 i;
  } u;
  u.f = val;
  return Serialize(u.i);
}

template <>
[[nodiscard]] inline constexpr f32 Deserialize(
    std::array<std::byte, sizeof(f32)> data) {
  union {
    f32 f;
    i32 i;
  } u;
  u.i = Deserialize<i32>(data);
  return u.f;
}

template <>
[[nodiscard]] inline constexpr f64 Deserialize(
    std::array<std::byte, sizeof(f64)> data) {
  union {
    f64 f;
    i64 i;
  } u;
  u.i = Deserialize<i64>(data);
  return u.f;
}

}  // namespace srpc

#endif  // SRPC_PROTOCOL_FLOATS_H_
