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
  auto data1 = Marshal<std::vector<i32>>{}(vec1);
  auto res1 = Unmarshal<std::vector<i32>>{}(data1);
  ASSERT_EQ(data1.size(), res1.first);
  ASSERT_TRUE(res1.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert_eq(vec1, *res1.second);

  std::vector<u64> vec2{0xdeadbeefdeadbeef, 0xbaaaaaaaaaaaaaad,
                        0xbaadf00dbaadf00d, 0x2002020520020205};
  auto data2 = Marshal<std::vector<u64>>{}(vec2);
  auto res2 = Unmarshal<std::vector<u64>>{}(data2);
  ASSERT_EQ(data2.size(), res2.first);
  ASSERT_TRUE(res2.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert_eq(vec2, *res2.second);

  std::vector<f64> vec3{1.6180339887, 2.7182818284, 3.1415926535};
  auto data3 = Marshal<std::vector<f64>>{}(vec3);
  auto res3 = Unmarshal<std::vector<f64>>{}(data3);
  ASSERT_EQ(data3.size(), res3.first);
  ASSERT_TRUE(res3.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert_eq(vec3, *res3.second);

  std::vector<std::string> vec4{"lorem", "ipsum", "dolor", "sit", "amet"};
  auto data4 = Marshal<std::vector<std::string>>{}(vec4);
  auto res4 = Unmarshal<std::vector<std::string>>{}(data4);
  ASSERT_EQ(data4.size(), res4.first);
  ASSERT_TRUE(res4.second.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  assert_eq(vec4, *res4.second);
}
