#ifndef JellyVM_h
#define JellyVM_h

#include <string>
#include <vector>

#include "../Logger.h"
#include "../bytecode/OpCode.h"

/**
 * Reads the current byte in the bytecode
 * and advances the ip pointer
 */
#define READ_BYTE() *ip++

/**
 * Jelly Virtual Machine.
 */
class JellyVM {
public:
  JellyVM() {}

  /**
   * Executes a program.
   */
  void exec(const std::string &program) {
    // 1. Parse the program.
    // auto ast = parser->parse(program)

    // 2.Compile program to Jelly bytecode.
    // code = compiler->compile(ast)
    code = {OP_HALT};

    // Set instruction pointer to the beginning.
    ip = &code[0];

    return eval();
  }

  /**
   * Main eval loop.
   */
  void eval() {
    for (;;) {
      auto opcode = READ_BYTE();
      switch (opcode) {
      case OP_HALT:
        return;
      default:
        DIE << "Unknown opcode: " << std::hex << (int)opcode;
      }
    }
  }

  /**
   * Instruction pointer (aka Program Counter)
   */
  uint8_t *ip;

  /**
   * Bytecode.
   */
  std::vector<uint8_t> code;
};

#endif