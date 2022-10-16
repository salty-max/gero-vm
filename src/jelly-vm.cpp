#include "vm/JellyVM.h"

/**
 * Jelly VM main executable.
 */
int main(int argc, char const *argv[]) {
  JellyVM vm;

  vm.exec(R"(
    42
  )");

  return 0;
}
