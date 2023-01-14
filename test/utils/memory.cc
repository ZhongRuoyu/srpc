#include "utils/memory.h"

#include <array>
#include <cstddef>

#include <gtest/gtest.h>

template <typename... Ts>
static constexpr std::array<std::byte, sizeof...(Ts)> MakeBytes(Ts... bytes) {
  return std::array<std::byte, sizeof...(Ts)>{std::byte(bytes)...};
};

TEST(Utils, Memcpy) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  auto check = [&assert_eq](auto data) {
    std::array<std::byte, data.size()> tmp;
    auto result = srpc::Memcpy(tmp.data(), data.data(), data.size());
    assert_eq(data, tmp);
    ASSERT_EQ(tmp.data(), result);
  };
  check(MakeBytes(0xde, 0xad, 0xbe, 0xef));
  check(MakeBytes('h', 'e', 'l', 'l', 'o'));
  check(MakeBytes());
}

template <typename T, std::size_t N>
static constexpr bool CheckMemcpyIsConstexpr(std::array<T, N> arr) {
  std::array<T, N> tmp;
  auto result = srpc::Memcpy(tmp.data(), arr.data(), arr.size());
  if (result != tmp.data()) {
    return false;
  }
  for (std::size_t i = 0; i < N; ++i) {
    if (tmp[i] != arr[i]) {
      return false;
    }
  }
  return true;
}

TEST(Utils, MemcpyIsConstexpr) {
  constexpr bool check1 =
      CheckMemcpyIsConstexpr(MakeBytes('w', 'o', 'r', 'l', 'd'));
  ASSERT_TRUE(check1);

  constexpr bool check2 = CheckMemcpyIsConstexpr(MakeBytes(31, 41, 59, 26));
  ASSERT_TRUE(check2);

  constexpr bool check3 = CheckMemcpyIsConstexpr(MakeBytes());
  ASSERT_TRUE(check3);
}
