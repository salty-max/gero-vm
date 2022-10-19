/**
 * Instruction set for Jelly VM.
 */

#ifndef OpCode_h
#define OpCode_h

#include "../Logger.h"

/**
 * Stops the program.
 */
#define OP_HALT 0x00

/**
 * Pushes a const onto the stack.
 */
#define OP_CONST 0x01

/**
 * Math instructions.
 */
#define OP_ADD 0x02
#define OP_SUB 0x03
#define OP_MUL 0x04
#define OP_DIV 0x05
#define OP_MOD 0x06

/**
 * Comparisons.
 */
#define OP_COMPARE 0x07

/**
 * Control flow: jump if the value on the stack is false.
 */
#define OP_JMP_IF_FALSE 0x08

/**
 * Unconditional jump.
 */
#define OP_JMP 0x09

/**
 * Returns global variable.
 */
#define OP_GET_GLOBAL 0x0A

/**
 * Sets global variable value.
 */
#define OP_SET_GLOBAL 0x0B

/**
 * Pops a vlue from the stack.
 */
#define OP_POP 0x0C

// -----------------------------------------------------------

#define OP_STR(op)                                                             \
  case OP_##op:                                                                \
    return #op;

std::string opcodeToString(uint8_t opcode) {
  switch (opcode) {
    OP_STR(HALT);
    OP_STR(CONST);
    OP_STR(ADD);
    OP_STR(SUB);
    OP_STR(MUL);
    OP_STR(DIV);
    OP_STR(MOD);
    OP_STR(COMPARE);
    OP_STR(JMP_IF_FALSE);
    OP_STR(JMP);
    OP_STR(GET_GLOBAL);
    OP_STR(SET_GLOBAL);
    OP_STR(POP);
  default:
    DIE << "opcodeToString: unknown opcode: " << (int)opcode;
  }

  return "Unknown"; // Unreachable
}

#endif