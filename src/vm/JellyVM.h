#ifndef JellyVM_h
#define JellyVM_h

#include <array>
#include <string>
#include <vector>

#include "../Logger.h"
#include "../bytecode/OpCode.h"
#include "../compiler/JellyCompiler.h"
#include "../parser/JellyParser.h"
#include "./Global.h"
#include "./JellyValue.h"

using syntax::JellyParser;

/**
 * Reads the current byte in the bytecode
 * and advances the ip pointer
 */
#define READ_BYTE() *ip++

/**
 * Reads a short word (2 bytes)
 */
#define READ_SHORT() (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))

/**
 * Converts bytecode index to a pointer.
 */
#define TO_ADDRESS(index) (&co->code[index])

/**
 * Gets a constant from the pool.
 */
#define GET_CONST() (co->constants[READ_BYTE()])

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
      : global(std::make_shared<Global>()),
        parser(std::make_unique<JellyParser>()),
        compiler(std::make_unique<JellyCompiler>(global)) {
    setGlobalVariables();
  }

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
   * Pops multiple values from the stack.
   */
  void popN(size_t count) {
    if (stack.size() == 0) {
      DIE << "popN(): stack underflow.\n";
    }

    sp -= count;
  }

  /**
   * Peeks an element from the stack.
   */
  JellyValue peek(size_t offset = 0) {
    if (stack.size() == 0) {
      DIE << "peek(): stack underflow.\n";
    }

    return *(sp - 1 - offset);
  }

  /**
   * Executes a program.
   */
  JellyValue exec(const std::string &program) {
    // 1. Parse the program.
    auto ast = parser->parse("(begin " + program + ")");

    // 2.Compile program to Jelly bytecode.
    co = compiler->compile(ast);

    // Set instruction pointer to the beginning.
    ip = &co->code[0];

    // Set stack pointer to the beginning.
    sp = &stack[0];

    // Init the base pointer.
    bp = sp;

    // Debug disassembly
    compiler->disassembleBytecode();

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
      // Constants.
      case OP_CONST:
        push(GET_CONST());
        break;

      // ------------------------
      // Math ops.
      case OP_ADD: {
        auto op2 = pop();
        auto op1 = pop();

        // Numeric addition.
        if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
          auto v1 = AS_NUMBER(op1);
          auto v2 = AS_NUMBER(op2);
          push(NUMBER(v1 + v2));
        }

        // String concatenation.
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
      // Comparison.
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

      // ------------------------
      // Conditional jump.
      case OP_JMP_IF_FALSE: {
        auto cond = AS_BOOLEAN(pop());

        auto address = READ_SHORT();

        if (!cond) {
          ip = TO_ADDRESS(address);
        }

        break;
      }

      // ------------------------
      // Unconditional jump.
      case OP_JMP: {
        ip = TO_ADDRESS(READ_SHORT());
        break;
      }

      // ------------------------
      // Global variable value.
      case OP_GET_GLOBAL: {
        auto globalIndex = READ_BYTE();
        push(global->get(globalIndex).value);
        break;
      }

      // ------------------------
      // Global variable assignment.
      case OP_SET_GLOBAL: {
        auto globalIndex = READ_BYTE();
        auto value = peek(0);
        global->set(globalIndex, value);
        break;
      }

      // ------------------------
      // Local variable value.
      case OP_GET_LOCAL: {
        auto localIndex = READ_BYTE();

        if (localIndex < 0 || localIndex >= stack.size()) {
          DIE << "OP_GET_LOCAL: invalid variable index: " << (int)localIndex;
        }
        push(bp[localIndex]);

        break;
      }

      // ------------------------
      // Local variable assignment.
      case OP_SET_LOCAL: {
        auto localIndex = READ_BYTE();

        if (localIndex < 0 || localIndex >= stack.size()) {
          DIE << "OP_GET_LOCAL: invalid variable index: " << (int)localIndex;
        }

        auto value = peek(0);
        bp[localIndex] = value;
        break;
      }

      // ------------------------
      // Stack manipulation.
      case OP_POP:
        pop();
        break;

      // ------------------------
      // Scope exit (clean up variables).
      //
      // NOTE: Variables sit right below the result of a block,
      // so we move the result below, which will be the new top
      // after popping the variables.
      case OP_SCOPE_EXIT: {
        // How many vars to pop.
        auto count = READ_BYTE();

        // Move the result above the vars.
        *(sp - 1 - count) = peek(0);

        // Pop the vars.
        popN(count);
        break;
      }

      default:
        DIE << "Unknown opcode: " << std::hex << (int)opcode;
      }
    }
  }

  /**
   * Sets up global variables and functions.
   */
  void setGlobalVariables() {
    global->addConst("PI", 3.141592653589793);
    global->addConst("THE_ANSWER", 42);
    global->addConst("VERSION", 0.1);
  }

  /**
   * Global object.
   */
  std::shared_ptr<Global> global;

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
   * Base pointer (Frame pointer).
   */
  JellyValue *bp;

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