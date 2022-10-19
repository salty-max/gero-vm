#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class BlockTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  BlockTest() {
    // You can do set-up work for each test here.
  }

  ~BlockTest() override {
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

TEST(BlockTest, GlobalBlockWithMultipleExpression) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var x 5)
    (set x (* x x))
    x
  )");

  EXPECT_EQ(AS_NUMBER(result), 25);
}

TEST(BlockTest, NestedBlocks) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var x 5)

    (begin
      (var x 10)
      x
    )

    x
  )");

  EXPECT_EQ(AS_NUMBER(result), 5);
}