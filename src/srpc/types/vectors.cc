#include "srpc/types/vectors.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

std::vector<std::byte> Marshal<std::vector<std::vector<std::byte>>>::operator()(
    const std::vector<std::vector<std::byte>> &vec) const {
  std::vector<std::byte> data(sizeof(i64) * (vec.size() + 1));
  Marshal<i64>{}(std::ssize(vec), std::span<std::byte, sizeof(i64)>{
                                      data.data(), data.data() + sizeof(i64)});

  i64 total_len = 0;
  for (i64 i = 0; i < vec.size(); ++i) {
    Marshal<i64>{}(std::ssize(vec[i]),
                   std::span<std::byte, sizeof(i64)>{
                       data.data() + sizeof(i64) * (1 + i),
                       data.data() + sizeof(i64) * (1 + i + 1)});
    total_len += std::ssize(vec[i]);
  }

  data.reserve(sizeof(i64) * (vec.size() + 1) + total_len);
  for (const auto &element : vec) {
    data.insert(data.end(), element.begin(), element.end());
  }
  return data;
}

std::pair<i64, std::optional<std::vector<std::vector<std::byte>>>>
Unmarshal<std::vector<std::vector<std::byte>>>::operator()(
    std::span<const std::byte> data) const {
  if (data.size() < sizeof(i64)) {
    return {0, {}};
  }

  auto len = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
      data.data(), data.data() + sizeof(i64)});
  if (data.size() < sizeof(i64) * (1 + len)) {
    return {0, {}};
  }

  std::vector<i64> element_lens;
  element_lens.reserve(len);
  i64 total_len = 0;
  for (i64 i = 0; i < len; ++i) {
    auto element_len = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
        data.data() + sizeof(i64) * (1 + i),
        data.data() + sizeof(i64) * (1 + i + 1)});
    element_lens.emplace_back(element_len);
    total_len += element_len;
  }
  if (data.size() < sizeof(i64) * (1 + len) + total_len) {
    return {0, {}};
  }

  std::vector<std::vector<std::byte>> result;
  result.reserve(len);
  i64 p = static_cast<i64>(sizeof(i64) * (1 + len));
  for (i64 i = 0; i < len; ++i) {
    result.emplace_back(data.begin() + p, data.begin() + p + element_lens[i]);
    p += element_lens[i];
  }
  return {p, result};
}

std::vector<std::byte> Marshal<std::vector<std::string>>::operator()(
    const std::vector<std::string> &vec) const {
  std::vector<std::byte> data(sizeof(i64) * (vec.size() + 1));
  Marshal<i64>{}(std::ssize(vec), std::span<std::byte, sizeof(i64)>{
                                      data.data(), data.data() + sizeof(i64)});

  i64 total_len = 0;
  for (i64 i = 0; i < vec.size(); ++i) {
    Marshal<i64>{}(std::ssize(vec[i]),
                   std::span<std::byte, sizeof(i64)>{
                       data.data() + sizeof(i64) * (1 + i),
                       data.data() + sizeof(i64) * (1 + i + 1)});
    total_len += std::ssize(vec[i]);
  }

  data.reserve(sizeof(i64) * (vec.size() + 1) + total_len);
  for (const auto &str : vec) {
    std::transform(str.begin(), str.end(), std::back_inserter(data),
                   [](char c) { return std::byte(c); });
  }
  return data;
}

std::pair<i64, std::optional<std::vector<std::string>>>
Unmarshal<std::vector<std::string>>::operator()(
    std::span<const std::byte> data) const {
  if (data.size() < sizeof(i64)) {
    return {0, {}};
  }

  auto len = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
      data.data(), data.data() + sizeof(i64)});
  if (data.size() < sizeof(i64) * (1 + len)) {
    return {0, {}};
  }

  std::vector<i64> str_lens;
  str_lens.reserve(len);
  i64 total_len = 0;
  for (i64 i = 0; i < len; ++i) {
    auto str_len = Unmarshal<i64>{}(std::span<const std::byte, sizeof(i64)>{
        data.data() + sizeof(i64) * (1 + i),
        data.data() + sizeof(i64) * (1 + i + 1)});
    str_lens.emplace_back(str_len);
    total_len += str_len;
  }
  if (data.size() != sizeof(i64) * (1 + len) + total_len) {
    return {0, {}};
  }

  std::vector<std::string> result;
  result.reserve(len);
  i64 p = static_cast<i64>(sizeof(i64) * (1 + len));
  for (i64 i = 0; i < len; ++i) {
    std::string str;
    std::transform(data.begin() + p, data.begin() + p + str_lens[i],
                   std::back_inserter(str),
                   [](std::byte b) { return char(b); });
    result.emplace_back(std::move(str));
    p += str_lens[i];
  }
  return {p, result};
}

}  // namespace srpc
