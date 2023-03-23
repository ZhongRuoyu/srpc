#include "srpc/types/vectors.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "srpc/types/floats.h"
#include "srpc/types/integers.h"
#include "srpc/types/serialization.h"

using namespace srpc;

TEST(Protocol, MarshalAndUnmarshalVectors) {
  auto assert_eq = [](auto expected, auto actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  };

  std::vector<i32> vec1{31, 41, 59, 26, 53, 58};
  auto res1 = Unmarshal<std::vector<i32>>{}(Marshal<std::vector<i32>>{}(vec1));
  ASSERT_TRUE(res1.has_value());
  assert_eq(vec1, *res1);  // NOLINT(bugprone-unchecked-optional-access)

  std::vector<u64> vec2{0xdeadbeefdeadbeef, 0xbaaaaaaaaaaaaaad,
                        0xbaadf00dbaadf00d, 0x2002020520020205};
  auto res2 = Unmarshal<std::vector<u64>>{}(Marshal<std::vector<u64>>{}(vec2));
  ASSERT_TRUE(res2.has_value());
  assert_eq(vec2, *res2);  // NOLINT(bugprone-unchecked-optional-access)

  std::vector<f64> vec3{1.6180339887, 2.7182818284, 3.1415926535};
  auto res3 = Unmarshal<std::vector<f64>>{}(Marshal<std::vector<f64>>{}(vec3));
  ASSERT_TRUE(res3.has_value());
  assert_eq(vec3, *res3);  // NOLINT(bugprone-unchecked-optional-access)

  std::vector<std::string> vec4{"lorem", "ipsum", "dolor", "sit", "amet"};
  auto res4 = Unmarshal<std::vector<std::string>>{}(
      Marshal<std::vector<std::string>>{}(vec4));
  ASSERT_TRUE(res4.has_value());
  assert_eq(vec4, *res4);  // NOLINT(bugprone-unchecked-optional-access)
}
