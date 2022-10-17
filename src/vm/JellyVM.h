#ifndef JellyVM_h
#define JellyVM_h

#include <array>
#include <string>
#include <vector>

#include "../Logger.h"
#include "../bytecode/OpCode.h"
#include "./JellyValue.h"

/**
 * Reads the current byte in the bytecode
 * and advances the ip pointer
 */
#define READ_BYTE() *ip++

/**
 * Gets a constant from the pool.
 */
#define GET_CONST() constants[READ_BYTE()]

/**
 * Stack top (stack overflox after exceeding)
 */
#define STACK_LIMIT 512

/**
 * Jelly Virtual Machine.
 */
class JellyVM {
public:
  JellyVM() {}

  /**
   * Pushes a value onto the stack.
   */
  void push(const JellyValue &value) {
    if (((size_t)(sp - stack.begin())) == STACK_LIMIT) {
      DIE << "push(): Stack overflow.\n";
    }

    *sp = value;
    sp++;
  }

  /**
   * Pops a value from the stack.
   */
  JellyValue pop() {
    if (sp == stack.begin()) {
      DIE << "pop(): empty stack.\n";
    }
    // As the stack pointer points to the free slot
    // Decrement it to get to the top of th stack.
    --sp;
    return *sp;
  }

  /**
   * Executes a program.
   */
  JellyValue exec(const std::string &program) {
    // 1. Parse the program.
    // auto ast = parser->parse(program)

    // 2.Compile program to Jelly bytecode.
    // code = compiler->compile(ast)
    constants.push_back(NUMBER(42));
    code = {OP_CONST, 0, OP_HALT};

    // Set instruction pointer to the beginning.
    ip = &code[0];

    // Set stack pointer to the beginning.
    sp = &stack[0];

    return eval();
  }

  /**
   * Main eval loop.
   */
  JellyValue eval() {
    for (;;) {
      auto opcode = READ_BYTE();
      switch (opcode) {
      case OP_HALT:
        return pop();
      // ------------------------
      // Constants
      case OP_CONST:
        push(GET_CONST());
        break;
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
   * Stack pointer.
   */
  JellyValue *sp;

  /**
   * Operands stack.
   */
  std::array<JellyValue, STACK_LIMIT> stack;

  /**
   * Constant pool.
   */
  std::vector<JellyValue> constants;
  /**
   * Bytecode.
   */
  std::vector<uint8_t> code;
};

#endif