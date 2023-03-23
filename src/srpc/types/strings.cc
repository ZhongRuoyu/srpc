#include "srpc/types/strings.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

std::vector<std::byte> Marshal<std::string>::operator()(
    const std::string &str) const {
  std::vector<std::byte> data(sizeof(i64) + str.size());
  Marshal<i64>{}(std::ssize(str), std::span<std::byte, sizeof(i64)>{
                                      data.data(), data.data() + sizeof(i64)});
  std::transform(str.begin(), str.end(), data.begin() + sizeof(i64),
                 [](char c) { return std::byte(c); });
  return data;
}

std::optional<std::string> Unmarshal<std::string>::operator()(
    const std::span<const std::byte> &data) const {
  if (data.size() < sizeof(i64)) {
    return {};
  }

  auto len = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
      data.data(), data.data() + sizeof(i64)});
  if (data.size() != sizeof(i64) + len) {
    return {};
  }

  std::string str;
  std::transform(data.begin() + sizeof(i64), data.end(),
                 std::back_inserter(str), [](std::byte b) { return char(b); });
  return str;
}

}  // namespace srpc
