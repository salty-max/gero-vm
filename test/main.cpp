#include "gtest/gtest.h"

#include "binary-math-test.h"
#include "expression-test.h"
#include "string-test.h"
// #include "string.h"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
