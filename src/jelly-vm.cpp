#include <iostream>

#include "./Logger.h"
#include "vm/JellyVM.h"

/**
 * Jelly VM main executable.
 */
int main(int argc, char const *argv[]) {
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

  log(result);

  std::cout << "👌 All done!\n";

  return 0;
}
