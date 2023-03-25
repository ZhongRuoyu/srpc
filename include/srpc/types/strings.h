#ifndef SRPC_TYPES_STRINGS_H_
#define SRPC_TYPES_STRINGS_H_

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

template <>
struct Marshal<std::string> {
  [[nodiscard]] std::vector<std::byte> operator()(const std::string &str) const;
};

template <>
struct Unmarshal<std::string> {
  [[nodiscard]] std::pair<i64, std::optional<std::string>> operator()(
      std::span<const std::byte> data) const;
};

}  // namespace srpc

#endif  // SRPC_TYPES_STRINGS_H_
