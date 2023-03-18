#include "srpc/utils/result.h"

#include <memory>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "srpc/types/integers.h"

using namespace srpc;

TEST(Utils, Result) {
  Result<i32> result1 = 42;
  ASSERT_TRUE(result1.OK());
  ASSERT_EQ(result1.Value(), 42);

  Result<i32> result2 = std::string{"error test"};
  ASSERT_FALSE(result2.OK());
  ASSERT_EQ(result2.Error(), "error test");

  Result<i32, i64> result3 = 3141592;
  ASSERT_TRUE(result3.OK());
  ASSERT_EQ(result3.Value(), 3141592);

  Result<i32, i64> result4 = 6535897LL;
  ASSERT_FALSE(result4.OK());
  ASSERT_EQ(result4.Error(), 6535897);

  Result<std::unique_ptr<int>> result5 = std::make_unique<int>(2023);
  ASSERT_TRUE(result5.OK());
  ASSERT_NE(result5.Value(), nullptr);
  ASSERT_EQ(*result5.Value(), 2023);
  auto value_moved = std::move(result5.Value());
  ASSERT_TRUE(result5.OK());
  ASSERT_EQ(result5.Value(), nullptr);
  ASSERT_NE(value_moved, nullptr);
  ASSERT_EQ(*value_moved, 2023);

  Result<bool, std::unique_ptr<std::string>> result6 =
      std::make_unique<std::string>("hello, world");
  ASSERT_FALSE(result6.OK());
  ASSERT_NE(result6.Error(), nullptr);
  ASSERT_EQ(*result6.Error(), "hello, world");
  auto error_moved = std::move(result6.Error());
  ASSERT_FALSE(result6.OK());
  ASSERT_EQ(result6.Error(), nullptr);
  ASSERT_NE(error_moved, nullptr);
  ASSERT_EQ(*error_moved, "hello, world");
}
