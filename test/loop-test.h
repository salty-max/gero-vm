#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class LoopTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  LoopTest() {
    // You can do set-up work for each test here.
  }

  ~LoopTest() override {
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

TEST(LoopTest, WhileLoop) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var i 10)
    (var count 0)

    (while (> i 0)
      (begin
        (set i (- i 1))
        (set count (+ count 1))
      )
    )

    count
  )");

  EXPECT_EQ(AS_NUMBER(result), 10);
}

TEST(LoopTest, ForLoop) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var count 0)

    (for (var i 10) (> i 0) (set i (- i 1))
      (begin
        (set count (+ count 1))
      )
    )

    count
  )");

  EXPECT_EQ(AS_NUMBER(result), 10);
}