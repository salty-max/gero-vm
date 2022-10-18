#ifndef JellyVM_h
#define JellyVM_h

#include <array>
#include <string>
#include <vector>

#include "../Logger.h"
#include "../bytecode/OpCode.h"
#include "../compiler/JellyCompiler.h"
#include "../parser/JellyParser.h"
#include "./JellyValue.h"

using syntax::JellyParser;

/**
 * Reads the current byte in the bytecode
 * and advances the ip pointer
 */
#define READ_BYTE() *ip++

/**
 * Gets a constant from the pool.
 */
#define GET_CONST() co->constants[READ_BYTE()]

/**
 * Stack top (stack overflox after exceeding)
 */
#define STACK_LIMIT 512

/**
 * Binary operation.
 */
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    auto op2 = AS_NUMBER(pop());                                               \
    auto op1 = AS_NUMBER(pop());                                               \
    push(NUMBER(op1 op op2));                                                  \
  } while (false)

/**
 * Generic values comparison.
 */
#define COMPARE_VALUES(op, v1, v2)                                             \
  do {                                                                         \
    bool res;                                                                  \
    switch (op) {                                                              \
    case 0:                                                                    \
      res = v1 < v2;                                                           \
      break;                                                                   \
    case 1:                                                                    \
      res = v1 > v2;                                                           \
      break;                                                                   \
    case 2:                                                                    \
      res = v1 == v2;                                                          \
      break;                                                                   \
    case 3:                                                                    \
      res = v1 <= v2;                                                          \
      break;                                                                   \
    case 4:                                                                    \
      res = v1 >= v2;                                                          \
      break;                                                                   \
    case 5:                                                                    \
      res = v1 != v2;                                                          \
      break;                                                                   \
    }                                                                          \
    push(BOOLEAN(res));                                                        \
  } while (false)

/**
 * Jelly Virtual Machine.
 */
class JellyVM {
public:
  JellyVM()
      : parser(std::make_unique<JellyParser>()),
        compiler(std::make_unique<JellyCompiler>()) {}

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
      DIE << "pop(): Stack underflow.\n";
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
    auto ast = parser->parse(program);

    // 2.Compile program to Jelly bytecode.
    co = compiler->compile(ast);

    // Set instruction pointer to the beginning.
    ip = &co->code[0];

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

      // ------------------------
      // Math ops
      case OP_ADD: {
        auto op2 = pop();
        auto op1 = pop();

        // Numeric addition:
        if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
          auto v1 = AS_NUMBER(op1);
          auto v2 = AS_NUMBER(op2);
          push(NUMBER(v1 + v2));
        }

        // String concatenation
        else if (IS_STRING(op1) && IS_STRING(op2)) {
          auto s1 = AS_CPPSTRING(op1);
          auto s2 = AS_CPPSTRING(op2);
          push(ALLOC_STRING(s1 + s2));
        }
        break;
      }
      case OP_SUB: {
        BINARY_OP(-);
        break;
      }
      case OP_MUL: {
        BINARY_OP(*);
        break;
      }
      case OP_DIV: {
        BINARY_OP(/);
        break;
      }
      case OP_MOD: {
        auto op2 = (int)(AS_NUMBER(pop()));
        auto op1 = (int)(AS_NUMBER(pop()));
        push(NUMBER((double)(op1 % op2)));
        break;
      }

      // ------------------------
      // Comparison
      case OP_COMPARE: {
        auto op = READ_BYTE();
        auto op2 = pop();
        auto op1 = pop();

        if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
          auto v1 = AS_NUMBER(op1);
          auto v2 = AS_NUMBER(op2);
          COMPARE_VALUES(op, v1, v2);
        } else if (IS_STRING(op1) && IS_STRING(op2)) {
          auto s1 = AS_STRING(op1);
          auto s2 = AS_STRING(op2);
          COMPARE_VALUES(op, s1, s2);
        }
        break;
      }

      default:
        DIE << "Unknown opcode: " << std::hex << (int)opcode;
      }
    }
  }

  /**
   * Parser.
   */
  std::unique_ptr<JellyParser> parser;

  /**
   * Compiler.
   */
  std::unique_ptr<JellyCompiler> compiler;

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
   * Code object.
   */
  CodeObject *co;
};

#endif