#ifndef SRPC_PROTOCOL_STRINGS_H_
#define SRPC_PROTOCOL_STRINGS_H_

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "srpc/types/serialization.h"

namespace srpc {

template <>
struct Marshal<std::string> {
  [[nodiscard]] std::vector<std::byte> operator()(const std::string &str) const;
};

template <>
struct Unmarshal<std::string> {
  [[nodiscard]] std::optional<std::string> operator()(
      const std::vector<std::byte> &data) const;
};

}  // namespace srpc

#endif  // SRPC_PROTOCOL_STRINGS_H_
