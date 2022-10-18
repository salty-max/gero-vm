#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class BinaryMathTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  BinaryMathTest() {
    // You can do set-up work for each test here.
  }

  ~BinaryMathTest() override {
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

TEST(BinaryMathTest, Addition) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (+ 1 2)
  )");

  EXPECT_EQ(AS_NUMBER(result), 3);
}

TEST(BinaryMathTest, Substraction) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (- 2 1)
  )");

  EXPECT_EQ(AS_NUMBER(result), 1);
}

TEST(BinaryMathTest, Multiplication) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (* 2 2)
  )");

  EXPECT_EQ(AS_NUMBER(result), 4);
}

TEST(BinaryMathTest, Division) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (/ 20 5)
  )");

  EXPECT_EQ(AS_NUMBER(result), 4);
}

TEST(BinaryMathTest, Modulo) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (% 7 3)
  )");

  EXPECT_EQ(AS_NUMBER(result), 1);
}