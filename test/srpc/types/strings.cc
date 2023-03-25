#include "srpc/types/strings.h"

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "srpc/types/integers.h"

using namespace srpc;

template <typename... Ts>
static std::vector<std::byte> MakeBytes(Ts... bytes) {
  return std::vector<std::byte>{std::byte(bytes)...};
};

TEST(Protocol, MarshalStrings) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                      'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
            Marshal<std::string>{}("hello, world"));
  assert_eq(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0), Marshal<std::string>{}(""));
}

TEST(Protocol, UnmarshalStrings) {
  auto data1 = MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h',
                         'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd');
  auto res1 = Unmarshal<std::string>{}(data1);
  ASSERT_EQ(data1.size(), res1.first);
  ASSERT_TRUE(res1.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  ASSERT_EQ(std::string("hello, world"), *res1.second);

  auto data2 = MakeBytes(0, 0, 0, 0, 0, 0, 0, 0);
  auto res2 = Unmarshal<std::string>{}(data2);
  ASSERT_EQ(data2.size(), res2.first);
  ASSERT_TRUE(res2.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  ASSERT_EQ(std::string(""), *res2.second);
}
