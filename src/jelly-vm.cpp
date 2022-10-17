#include "vm/JellyVM.h"
#include <iostream>

/**
 * Jelly VM main executable.
 */
int main(int argc, char const *argv[]) {
  JellyVM vm;

  vm.exec(R"(
    42
  )");

  std::cout << "👌 All done!\n";

  return 0;
}
