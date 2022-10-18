#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class IfBranchTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  IfBranchTest() {
    // You can do set-up work for each test here.
  }

  ~IfBranchTest() override {
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

TEST(IfBranchTest, False) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (if (< 2 1) 1 2)
  )");

  EXPECT_EQ(AS_NUMBER(result), 2);
}

TEST(IfBranchTest, TrueWithAlternate) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (if (> 2 1) 1 2)
  )");

  EXPECT_EQ(AS_NUMBER(result), 1);
}
TEST(IfBranchTest, TrueWithoutAlternate) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (if (> 2 1) 1)
  )");

  EXPECT_EQ(AS_NUMBER(result), 1);
}