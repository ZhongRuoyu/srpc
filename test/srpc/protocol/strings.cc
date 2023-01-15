#include "srpc/protocol/strings.h"

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "srpc/protocol/integers.h"

using namespace srpc;

template <typename... Ts>
static std::vector<std::byte> MakeBytes(Ts... bytes) {
  return std::vector<std::byte>{std::byte(bytes)...};
};

TEST(Protocol, SerializeStrings) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                      'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
            Serialize(std::string("hello, world")));
  assert_eq(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0), Serialize(std::string("")));
}

TEST(Protocol, SerializeCStrings) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };

  assert_eq(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                      'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
            Serialize("hello, world"));
  assert_eq(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0), Serialize(""));

  char str1[]{"hello, world"};
  assert_eq(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0, 0, 'h', 'e',
                      'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd'),
            Serialize(str1));

  char str2[]{'s', 'r', 'p', 'c', '\0'};
  assert_eq(MakeBytes(4, 0, 0, 0, 0, 0, 0, 0, 's', 'r', 'p', 'c'),
            Serialize(str2));
}

TEST(Protocol, DeserializeStrings) {
  ASSERT_EQ(std::string("hello, world"),
            Deserialize(MakeBytes((u8)strlen("hello, world"), 0, 0, 0, 0, 0, 0,
                                  0, 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w',
                                  'o', 'r', 'l', 'd')));
  ASSERT_EQ(std::string(""), Deserialize(MakeBytes(0, 0, 0, 0, 0, 0, 0, 0)));
}
