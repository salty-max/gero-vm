#include "gtest/gtest.h"

#include "binary-math-test.h"
#include "block-test.h"
#include "comparison-test.h"
#include "expression-test.h"
#include "global-variable.h"
#include "if-branch-test.h"
#include "loop-test.h"
#include "string-test.h"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
