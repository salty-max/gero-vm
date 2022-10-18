#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class ExpressionTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  ExpressionTest() {
    // You can do set-up work for each test here.
  }

  ~ExpressionTest() override {
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

TEST(ExpressionTest, Number) {
  JellyVM vm;

  auto result = vm.exec(R"(
    42
  )");

  EXPECT_EQ(AS_NUMBER(result), 42);
}

TEST(ExpressionTest, String) {
  JellyVM vm;

  auto result = vm.exec(R"(
    "Hello World!"
  )");

  EXPECT_EQ(AS_CPPSTRING(result), "Hello World!");
}

TEST(ExpressionTest, List) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (+ 1 2)
  )");

  EXPECT_EQ(AS_NUMBER(result), 3);
}