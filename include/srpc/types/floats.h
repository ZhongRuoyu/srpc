#ifndef SRPC_TYPES_FLOATS_H_
#define SRPC_TYPES_FLOATS_H_

#include <array>
#include <cstddef>
#include <limits>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

// NOLINTBEGIN(readability-identifier-naming)

static_assert(std::numeric_limits<float>::is_iec559);
using f32 = float;

static_assert(std::numeric_limits<double>::is_iec559);
using f64 = double;

// NOLINTEND(readability-identifier-naming)

template <>
struct Marshal<f32> {
  [[nodiscard]] std::array<std::byte, sizeof(f32)> operator()(f32 val) const {
    return Marshal<i32>{}(*reinterpret_cast<i32 *>(&val));
  }
};

template <>
struct Marshal<f64> {
  [[nodiscard]] std::array<std::byte, sizeof(f64)> operator()(f64 val) const {
    return Marshal<i64>{}(*reinterpret_cast<i64 *>(&val));
  }
};

template <>
struct Unmarshal<f32> {
  [[nodiscard]] f32 operator()(std::array<std::byte, sizeof(f32)> data) const {
    i32 val = Unmarshal<i32>{}(data);
    return *reinterpret_cast<f32 *>(&val);
  }
};

template <>
struct Unmarshal<f64> {
  [[nodiscard]] f64 operator()(std::array<std::byte, sizeof(f64)> data) const {
    i64 val = Unmarshal<i64>{}(data);
    return *reinterpret_cast<f64 *>(&val);
  }
};

}  // namespace srpc

#endif  // SRPC_TYPES_FLOATS_H_
