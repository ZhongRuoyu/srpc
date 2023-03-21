#include "srpc/types/vectors.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

std::vector<std::byte> Marshal<std::vector<std::vector<std::byte>>>::operator()(
    const std::vector<std::vector<std::byte>> &vec) const {
  std::vector<std::byte> data;
  auto len = Marshal<i64>{}(std::ssize(vec));
  data.insert(data.end(), len.begin(), len.end());
  for (const auto &element : vec) {
    auto element_len = Marshal<i64>{}(std::ssize(element));
    data.insert(data.end(), element_len.begin(), element_len.end());
  }
  for (const auto &element : vec) {
    data.insert(data.end(), element.begin(), element.end());
  }
  return data;
}

std::optional<std::vector<std::vector<std::byte>>>
Unmarshal<std::vector<std::vector<std::byte>>>::operator()(
    const std::vector<std::byte> &data) const {
  if (data.size() < sizeof(i64)) {
    return {};
  }

  std::array<std::byte, sizeof(i64)> len_arr;
  std::copy(data.begin(), data.begin() + sizeof(i64), len_arr.begin());
  auto len = Unmarshal<i64>{}(len_arr);

  if (data.size() < sizeof(i64) * (1 + len)) {
    return {};
  }
  std::vector<i64> element_lens;
  element_lens.reserve(len);
  i64 total_len = 0;
  for (i64 i = 0; i < len; ++i) {
    std::array<std::byte, sizeof(i64)> element_len_arr;
    auto element_len_begin =
        data.begin() +
        static_cast<std::iter_difference_t<decltype(data.begin())>>(
            sizeof(i64) * (1 + i));
    std::copy(element_len_begin, element_len_begin + sizeof(i64),
              element_len_arr.begin());
    auto element_len = Unmarshal<i64>{}(element_len_arr);
    element_lens.emplace_back(element_len);
    total_len += element_len;
  }

  if (data.size() != sizeof(i64) * (1 + len) + total_len) {
    return {};
  }
  std::vector<std::vector<std::byte>> result;
  result.reserve(len);
  i64 p = static_cast<i64>(sizeof(i64) * (1 + len));
  for (i64 i = 0; i < len; ++i) {
    result.emplace_back(data.begin() + p, data.begin() + p + element_lens[i]);
    p += element_lens[i];
  }
  return result;
}

}  // namespace srpc
