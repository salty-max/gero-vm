#include <iostream>

#include "./Logger.h"
#include "vm/JellyVM.h"

/**
 * Jelly VM main executable.
 */
int main(int argc, char const *argv[]) {
  JellyVM vm;

  auto result = vm.exec(R"(
    (% (+ (* 6 5) 2) 5)
  )");

  log(AS_NUMBER(result));

  std::cout << "👌 All done!\n";

  return 0;
}
