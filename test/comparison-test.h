#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class ComparisonTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  ComparisonTest() {
    // You can do set-up work for each test here.
  }

  ~ComparisonTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

TEST(ComparisonTest, LessThan) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (< 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), true);
}

TEST(ComparisonTest, GreaterThan) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (> 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), false);
}

TEST(ComparisonTest, Equal) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (== 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), false);
}

TEST(ComparisonTest, LessThanOrEqual) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (<= 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), true);
}

TEST(ComparisonTest, GreaterThanOrEqual) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (>= 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), false);
}

TEST(ComparisonTest, NotEqual) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (!= 1 2)
  )");

  EXPECT_EQ(AS_BOOLEAN(result), true);
}
