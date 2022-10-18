#include <iostream>

#include "./Logger.h"
#include "vm/JellyVM.h"

/**
 * Jelly VM main executable.
 */
int main(int argc, char const *argv[]) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (+ "Hello " "World!")
  )");

  log(result);

  std::cout << "👌 All done!\n";

  return 0;
}
