#include "srpc/types/strings.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

std::vector<std::byte> Marshal<std::string>::operator()(
    const std::string &str) const {
  std::vector<std::byte> data;
  auto len = Marshal<i64>{}(std::ssize(str));
  data.reserve(len.size() + str.size());
  std::copy(len.begin(), len.end(), std::back_inserter(data));
  std::transform(str.begin(), str.end(), std::back_inserter(data),
                 [](char c) { return std::byte(c); });
  return data;
}

std::optional<std::string> Unmarshal<std::string>::operator()(
    const std::vector<std::byte> &data) const {
  if (data.size() < sizeof(i64)) {
    return {};
  }

  std::array<std::byte, sizeof(i64)> len_arr;
  std::copy(data.begin(), data.begin() + sizeof(i64), len_arr.begin());
  auto len = Unmarshal<i64>{}(len_arr);
  if (data.size() != sizeof(i64) + len) {
    return {};
  }

  std::string str;
  std::transform(data.begin() + sizeof(i64), data.end(),
                 std::back_inserter(str), [](std::byte b) { return char(b); });
  return str;
}

}  // namespace srpc
