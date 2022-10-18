#include "vm/JellyVM.h"
#include "gtest/gtest.h"

class GlobalVariableTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  GlobalVariableTest() {
    // You can do set-up work for each test here.
  }

  ~GlobalVariableTest() override {
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

TEST(GlobalVariableTest, GetGlobalVariable) {
  JellyVM vm;

  auto result = vm.exec(R"(
    THE_ANSWER
  )");

  EXPECT_EQ(AS_NUMBER(result), 42);
}

TEST(GlobalVariableTest, SetGlobalVariable) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var MARIGNAN 1515)
  )");

  EXPECT_EQ(AS_NUMBER(result), 1515);
}

TEST(GlobalVariableTest, SetGlobalVariableWithComplexExpression) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (var HELLO_WORLD (+ "Hello " "world!"))
  )");

  EXPECT_EQ(AS_CPPSTRING(result), "Hello world!");
}

TEST(GlobalVariableTest, UpdateGlobalVariable) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (set TOTO (* TOTO TOTO))
  )");

  EXPECT_EQ(AS_NUMBER(result), 4);
}