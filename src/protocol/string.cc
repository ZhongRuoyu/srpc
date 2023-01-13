#include "protocol/string.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include "protocol/integers.h"

namespace srpc {

std::vector<std::byte> Serialize(const std::string &str) {
  std::vector<std::byte> data;
  auto len = srpc::Serialize(srpc::i64(str.size()));
  data.reserve(len.size() + str.size());
  std::copy(len.begin(), len.end(), std::back_inserter(data));
  std::transform(str.begin(), str.end(), std::back_inserter(data),
                 [](char c) { return std::byte(c); });
  return data;
}

std::optional<std::string> Deserialize(const std::vector<std::byte> &data) {
  if (data.size() < sizeof(srpc::i64)) {
    return {};
  }

  std::array<std::byte, sizeof(srpc::i64)> len_arr;
  std::copy(data.begin(), data.begin() + sizeof(srpc::i64), len_arr.begin());
  auto len = srpc::Deserialize<srpc::i64>(len_arr);
  if (data.size() != sizeof(srpc::i64) + len) {
    return {};
  }

  std::string str;
  std::transform(data.begin() + sizeof(srpc::i64), data.end(),
                 std::back_inserter(str), [](std::byte b) { return char(b); });
  return str;
}

}  // namespace srpc
