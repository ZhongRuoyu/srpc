#ifndef SRPC_TYPES_VECTORS_H_
#define SRPC_TYPES_VECTORS_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

namespace srpc {

template <>
struct Marshal<std::vector<std::vector<std::byte>>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<std::vector<std::byte>> &vec) const;
};

template <>
struct Unmarshal<std::vector<std::vector<std::byte>>> {
  [[nodiscard]] std::pair<i64,
                          std::optional<std::vector<std::vector<std::byte>>>>
  operator()(std::span<const std::byte> data) const;
};

template <>
struct Marshal<std::vector<std::string>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<std::string> &vec) const;
};

template <>
struct Unmarshal<std::vector<std::string>> {
  [[nodiscard]] std::pair<i64, std::optional<std::vector<std::string>>>
  operator()(std::span<const std::byte> data) const;
};

template <typename T>
struct Marshal<
    std::vector<T>,
    typename std::enable_if_t<std::is_same_v<
        std::invoke_result_t<Marshal<T>, T>, std::vector<std::byte>>>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<T> &vec) const {
    std::vector<std::vector<std::byte>> vec_bytes;
    for (const auto &element : vec) {
      vec_bytes.emplace_back(Marshal<T>{}(element));
    }
    return Marshal<std::vector<std::vector<std::byte>>>{}(vec_bytes);
  }
};

template <typename T>
struct Marshal<std::vector<T>, typename std::enable_if_t<std::is_same_v<
                                   std::invoke_result_t<Marshal<T>, T>,
                                   std::array<std::byte, sizeof(T)>>>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<T> &vec) const {
    std::vector<std::vector<std::byte>> vec_bytes;
    for (const auto &element : vec) {
      auto element_len = Marshal<T>{}(element);
      vec_bytes.emplace_back(element_len.begin(), element_len.end());
    }
    return Marshal<std::vector<std::vector<std::byte>>>{}(vec_bytes);
  }
};

template <typename T>
struct Unmarshal<
    std::vector<T>,
    typename std::enable_if_t<std::is_same_v<
        std::invoke_result_t<Unmarshal<T>, std::span<const std::byte>>,
        std::optional<T>>>> {
  [[nodiscard]] std::pair<i64, std::optional<std::vector<T>>> operator()(
      std::span<const std::byte> data) const {
    auto maybe_vec = Unmarshal<std::vector<std::vector<std::byte>>>{}(data);
    if (!maybe_vec.second.has_value()) {
      return {0, {}};
    }

    std::vector<T> vec;
    for (const auto &element_bytes : *maybe_vec.second) {
      std::optional<T> maybe_element = Unmarshal<T>{}(element_bytes);
      if (!maybe_element.has_value()) {
        return {0, {}};
      }
      vec.emplace_back(std::move(*maybe_element));
    }
    return {maybe_vec.first, vec};
  }
};

template <typename T>
struct Unmarshal<
    std::vector<T>,
    typename std::enable_if_t<std::is_same_v<
        std::invoke_result_t<Unmarshal<T>, std::span<const std::byte>>,
        std::pair<i64, std::optional<T>>>>> {
  [[nodiscard]] std::pair<i64, std::optional<std::vector<T>>> operator()(
      std::span<const std::byte> data) const {
    auto maybe_vec = Unmarshal<std::vector<std::vector<std::byte>>>{}(data);
    if (!maybe_vec.second.has_value()) {
      return {0, {}};
    }

    std::vector<T> vec;
    for (const auto &element_bytes : *maybe_vec.second) {
      std::pair<i64, std::optional<T>> element_res =
          Unmarshal<T>{}(element_bytes);
      if (!element_res.second.has_value()) {
        return {0, {}};
      }
      if (element_res.first != element_bytes.size()) {
        return {0, {}};
      }
      vec.emplace_back(std::move(*element_res.second));
    }
    return {maybe_vec.first, vec};
  }
};

template <typename T>
struct Unmarshal<std::vector<T>,
                 typename std::enable_if_t<std::is_same_v<
                     std::invoke_result_t<
                         Unmarshal<T>, std::span<const std::byte, sizeof(T)>>,
                     T>>> {
  [[nodiscard]] std::pair<i64, std::optional<std::vector<T>>> operator()(
      std::span<const std::byte> data) const {
    auto res = Unmarshal<std::vector<std::vector<std::byte>>>{}(data);
    if (!res.second.has_value()) {
      return {0, {}};
    }

    std::vector<T> vec;
    for (const auto &element_bytes : *res.second) {
      if (element_bytes.size() != sizeof(T)) {
        return {0, {}};
      }
      T element = Unmarshal<T>{}(std::span<const std::byte, sizeof(T)>{
          element_bytes.data(), element_bytes.data() + sizeof(T)});
      vec.emplace_back(element);
    }
    return {res.first, vec};
  }
};

}  // namespace srpc

#endif  // SRPC_TYPES_VECTORS_H_
