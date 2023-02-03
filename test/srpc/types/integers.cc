#include "srpc/types/integers.h"

#include <array>
#include <cstddef>

#include <gtest/gtest.h>

using namespace srpc;

TEST(Protocol, ReverseBytesOfSignedInts) {
  ASSERT_EQ(i64(0x0df0ad8b0df0ad8b), ReverseBytes(i64(0x8badf00d8badf00d)));
  ASSERT_EQ(i32(0x0df0ad8b), ReverseBytes(i32(0x8badf00d)));
  ASSERT_EQ(i16(0x0df0), ReverseBytes(i16(0xf00d)));
  ASSERT_EQ(i8(0xf0), ReverseBytes(i8(0xf0)));
}

TEST(Protocol, ReverseBytesOfUnsignedInts) {
  ASSERT_EQ(u64(0xefbeaddeefbeadde), ReverseBytes(u64(0xdeadbeefdeadbeef)));
  ASSERT_EQ(u32(0xefbeadde), ReverseBytes(u32(0xdeadbeef)));
  ASSERT_EQ(u16(0xefbe), ReverseBytes(u16(0xbeef)));
  ASSERT_EQ(u8(0xbe), ReverseBytes(u8(0xbe)));
}

template <typename... Ts>
static constexpr std::array<std::byte, sizeof...(Ts)> MakeBytes(Ts... bytes) {
  return std::array<std::byte, sizeof...(Ts)>{std::byte(bytes)...};
};

TEST(Protocol, MarshalSignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde),
            Marshal<i64>{}(i64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde), Marshal<i32>{}(i32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), Marshal<i16>{}(i16(0xbeef)));
  assert_eq(MakeBytes(0xbe), Marshal<i8>{}(i8(0xbe)));
}

TEST(Protocol, MarshalUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde),
            Marshal<u64>{}(u64(0xdeadbeefdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe, 0xad, 0xde), Marshal<u32>{}(u32(0xdeadbeef)));
  assert_eq(MakeBytes(0xef, 0xbe), Marshal<u16>{}(u16(0xbeef)));
  assert_eq(MakeBytes(0xbe), Marshal<u8>{}(u8(0xbe)));
}

TEST(Protocol, UnmarshalSignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(i64(0xdeadbeefdeadbeef),
            Unmarshal<i64>{}(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(i32(0xdeadbeef),
            Unmarshal<i32>{}(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(i16(0xbeef), Unmarshal<i16>{}(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(i8(0xbe), Unmarshal<i8>{}(MakeBytes(0xbe)));
}

TEST(Protocol, UnmarshalUnsignedInts) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };
  ASSERT_EQ(u64(0xdeadbeefdeadbeef),
            Unmarshal<u64>{}(
                MakeBytes(0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(u32(0xdeadbeef),
            Unmarshal<u32>{}(MakeBytes(0xef, 0xbe, 0xad, 0xde)));
  ASSERT_EQ(u16(0xbeef), Unmarshal<u16>{}(MakeBytes(0xef, 0xbe)));
  ASSERT_EQ(u8(0xbe), Unmarshal<u8>{}(MakeBytes(0xbe)));
}
