#include "vm/GeroVM.h"

/**
 * Gero VM main executable.
 */
int main(int argc, char const *argv[]) {
  GeroVM vm;

  vm.exec(R"(
    42
  )");

  return 0;
}
