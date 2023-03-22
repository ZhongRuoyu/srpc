#ifndef SRPC_TYPES_VECTORS_H_
#define SRPC_TYPES_VECTORS_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "srpc/types/serialization.h"

namespace srpc {

template <>
struct Marshal<std::vector<std::vector<std::byte>>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<std::vector<std::byte>> &vec) const;
};

template <>
struct Unmarshal<std::vector<std::vector<std::byte>>> {
  [[nodiscard]] std::optional<std::vector<std::vector<std::byte>>> operator()(
      const std::vector<std::byte> &data) const;
};

template <>
struct Marshal<std::vector<std::string>> {
  [[nodiscard]] std::vector<std::byte> operator()(
      const std::vector<std::string> &vec) const;
};

template <>
struct Unmarshal<std::vector<std::string>> {
  [[nodiscard]] std::optional<std::vector<std::string>> operator()(
      const std::vector<std::byte> &data) const;
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
struct Unmarshal<std::vector<T>,
                 typename std::enable_if_t<std::is_same_v<
                     std::invoke_result_t<Unmarshal<T>, std::vector<std::byte>>,
                     std::optional<T>>>> {
  [[nodiscard]] std::optional<std::vector<T>> operator()(
      const std::vector<std::byte> &data) const {
    auto maybe_vec_bytes =
        Unmarshal<std::vector<std::vector<std::byte>>>{}(data);
    if (maybe_vec_bytes == std::nullopt) {
      return {};
    }

    std::vector<T> vec;
    for (const auto &element_bytes : *maybe_vec_bytes) {
      std::optional<T> maybe_element = Unmarshal<T>{}(element_bytes);
      if (maybe_element == std::nullopt) {
        return {};
      }
      vec.emplace_back(std::move(*maybe_element));
    }
    return vec;
  }
};

template <typename T>
struct Unmarshal<
    std::vector<T>,
    typename std::enable_if_t<std::is_same_v<
        std::invoke_result_t<Unmarshal<T>, std::array<std::byte, sizeof(T)>>,
        T>>> {
  [[nodiscard]] std::optional<std::vector<T>> operator()(
      const std::vector<std::byte> &data) const {
    auto maybe_vec_bytes =
        Unmarshal<std::vector<std::vector<std::byte>>>{}(data);
    if (maybe_vec_bytes == std::nullopt) {
      return {};
    }

    std::vector<T> vec;
    for (const auto &element_bytes : *maybe_vec_bytes) {
      if (element_bytes.size() != sizeof(T)) {
        return {};
      }
      std::array<std::byte, sizeof(T)> element_bytes_arr;
      std::copy(element_bytes.begin(), element_bytes.end(),
                element_bytes_arr.begin());
      T element = Unmarshal<T>{}(element_bytes_arr);
      vec.emplace_back(element);
    }
    return vec;
  }
};

}  // namespace srpc

#endif  // SRPC_TYPES_VECTORS_H_
