/**
 * Jelly compiler.
 */

#ifndef JellyCompiler_h
#define JellyCompiler_h

#include <map>
#include <string>

#include "../Logger.h"
#include "../bytecode/OpCode.h"
#include "../disassembler/JellyDisassembler.h"
#include "../parser/JellyParser.h"
#include "../vm/Global.h"
#include "../vm/JellyValue.h"

using syntax::JellyParser;

/**
 * Allocates new constant in the pool.
 */
#define ALLOC_CONST(tester, converter, allocator, value)                       \
  do {                                                                         \
    for (auto i = 0; i < co->constants.size(); i++) {                          \
      if (!tester(co->constants[i])) {                                         \
        continue;                                                              \
      }                                                                        \
      if (converter(co->constants[i]) == value) {                              \
        return i;                                                              \
      }                                                                        \
    }                                                                          \
    co->constants.push_back(allocator(value));                                 \
  } while (false)

// Generic binary operation: (+ 1 2) OP_CONST, OP_CONST, OP_ADD
#define GEN_BINARY_OP(op)                                                      \
  do {                                                                         \
    gen(exp.list[1]);                                                          \
    gen(exp.list[2]);                                                          \
    emit(op);                                                                  \
  } while (false)

/**
 * Compiler class, emits bytecode, records constant pool, vars, etc...
 */
class JellyCompiler {
public:
  JellyCompiler(std::shared_ptr<Global> global)
      : global(global),
        disassembler(std::make_unique<JellyDisassembler>(global)) {}

  /**
   * Main compile API.
   */
  CodeObject *compile(const Exp &exp) {
    // Allocate new code object
    co = AS_CODE(ALLOC_CODE("main"));

    // Generate recursively from top-level
    gen(exp);

    // Explicit VM-stop marker
    emit(OP_HALT);
    return co;
  }

  /**
   * Main compile loop.
   */
  void gen(const Exp &exp) {
    switch (exp.type) {
    /**
     * -----------------------------------
     * Numbers.
     */
    case ExpType::NUMBER:
      emit(OP_CONST);
      emit(numericConstIdx(exp.number));
      break;

    /**
     * -----------------------------------
     * Strings.
     */
    case ExpType::STRING:
      emit(OP_CONST);
      emit(stringConstIdx(exp.string));
      break;

    /**
     * -----------------------------------
     * Symbols (variables, operators).
     */
    case ExpType::SYMBOL:
      /**
       * Boolean.
       */
      if (exp.string == "true" || exp.string == "false") {
        emit(OP_CONST);
        emit(booleanConstIdx(exp.string == "true" ? true : false));
      } else {
        /**
         * Variables.
         */
        auto varName = exp.string;

        // 1. Local vars.
        auto localIndex = co->getLocalIndex(varName);

        if (localIndex != -1) {
          emit(OP_GET_LOCAL);
          emit(localIndex);
        }

        // 2. Global vars.
        else {
          if (!global->exists(varName)) {
            DIE << "[JellyCompiler]: Reference error: " << varName;
          }

          emit(OP_GET_GLOBAL);
          emit(global->getGlobalIndex(varName));
        }
      }
      break;

    /**
     * -----------------------------------
     * Lists.
     */
    case ExpType::LIST:
      auto tag = exp.list[0];

      /**
       * Special cases.
       */
      if (tag.type == ExpType::SYMBOL) {
        auto op = tag.string;

        /**
         * -------------------------------
         * Binary math operations (+ 1 2)
         */
        if (op == "+") {
          GEN_BINARY_OP(OP_ADD);
        } else if (op == "-") {
          GEN_BINARY_OP(OP_SUB);
        } else if (op == "*") {
          GEN_BINARY_OP(OP_MUL);
        } else if (op == "/") {
          GEN_BINARY_OP(OP_DIV);
        } else if (op == "%") {
          GEN_BINARY_OP(OP_MOD);
        }

        /**
         * -------------------------------
         * Compare operations (> 5 10)
         */
        else if (compareOps_.count(op) != 0) {
          gen(exp.list[1]);
          gen(exp.list[2]);
          emit(OP_COMPARE);
          emit(compareOps_[op]);
        }

        /**
         * -------------------------------
         * Branch instruction (if <test> <consequent> <alternate>)
         */
        else if (op == "if") {
          // Emit <test>
          gen(exp.list[1]);

          // If <test> returns false, skip <consequent>.
          emit(OP_JMP_IF_FALSE);

          // 2-byte address.
          emit(0);
          emit(0);

          auto elseJumpAddr = getOffset() - 2;

          // Emit <consequent>.
          gen(exp.list[2]);

          // Skip alternate part.
          emit(OP_JMP);

          // 2-byte address.
          emit(0);
          emit(0);

          auto endAddr = getOffset() - 2;

          // Patch the else branch address.
          auto elseBranchAddr = getOffset();
          patchJumpAddress(elseJumpAddr, elseBranchAddr);

          // Emit <alternate> if it exists.
          if (exp.list.size() == 4) {
            gen(exp.list[3]);
          } else {
            gen(NULL);
          }

          // Patch the end.
          auto endBranchAddr = getOffset();
          patchJumpAddress(endAddr, endBranchAddr);
        }

        /**
         * -------------------------------
         * While loop (while <test> <body>).
         */
        else if (op == "while") {
          auto loopStartAddr = getOffset();

          // Emit <test>
          gen(exp.list[1]);

          // <test> is false, loop ends. Jumps over the body.
          emit(OP_JMP_IF_FALSE);

          // 2-byte address
          emit(0);
          emit(0);

          auto loopEndJmpAddr = getOffset() - 2;

          // Emit <body>
          gen(exp.list[2]);

          // Goto loop start.
          emit(OP_JMP);

          emit(0);
          emit(0);

          patchJumpAddress(getOffset() - 2, loopStartAddr);

          // Patch the end.
          auto loopEndAddr = getOffset() + 1;
          patchJumpAddress(loopEndJmpAddr, loopEndAddr);
        }

        /**
         * -------------------------------
         * For loop (for <init> <test> <modifier> <body>).
         */
        else if (op == "for") {
          // Emit initialize
          gen(exp.list[1]);

          auto loopStartAddr = getOffset();

          // Emit <test>
          gen(exp.list[2]);

          // <test> is false, loop ends. Jumps over the body.
          emit(OP_JMP_IF_FALSE);

          // 2-byte address
          emit(0);
          emit(0);

          auto loopEndJmpAddr = getOffset() - 2;

          // Emit <body>
          gen(exp.list[4]);

          // Emit <modifier>
          gen(exp.list[3]);

          // Goto loop start.
          emit(OP_JMP);

          emit(0);
          emit(0);

          patchJumpAddress(getOffset() - 2, loopStartAddr);

          // Patch the end.
          auto loopEndAddr = getOffset() + 1;
          patchJumpAddress(loopEndJmpAddr, loopEndAddr);
        }

        /**
         * -------------------------------
         * Variable declaration (var x 2).
         */
        else if (op == "var") {
          auto varName = exp.list[1].string;

          // Initializer
          gen(exp.list[2]);

          // 1. Global vars.
          if (isGlobalScope()) {
            global->define(varName);

            emit(OP_SET_GLOBAL);
            emit(global->getGlobalIndex(varName));
          }

          // 2. Local vars.
          else {
            co->addLocal(varName);
            emit(OP_SET_LOCAL);
            emit(co->getLocalIndex(varName));
          }
        }

        /**
         * -------------------------------
         * Variable update (set x 2).
         */
        else if (op == "set") {
          auto varName = exp.list[1].string;

          // Value.
          gen(exp.list[2]);

          // 1. Local vars.
          auto localIndex = co->getLocalIndex(varName);

          if (localIndex != -1) {
            emit(OP_SET_LOCAL);
            emit(localIndex);
          }

          // 2. Global vars.
          else {
            auto globalIndex = global->getGlobalIndex(varName);
            if (globalIndex == -1) {
              DIE << "Reference error: " << varName << " is not defined.";
            }

            emit(OP_SET_GLOBAL);
            emit(globalIndex);
          }

        }

        /**
         * -------------------------------
         * Blocks (begin ).
         */
        else if (op == "begin") {
          // Increase scope level by 1.
          scopeEnter();

          // Compile each expression within the block.
          for (auto i = 1; i < exp.list.size(); i++) {
            // The value of the last expression is kept
            // on the stack as the final result.
            bool isLast = i == exp.list.size() - 1;

            // Local variable or function (should not pop).
            auto isLocalDeclaration =
                isDeclaration(exp.list[i]) && !isGlobalScope();

            // Generate expression code.
            gen(exp.list[i]);

            if (!isLast && !isLocalDeclaration) {
              emit(OP_POP);
            }
          }

          // Return to previous scope level.
          scopeExit();
        }
      }
      break;
    }
  }

  /**
   * Disassembles all compilation units.
   */
  void disassembleBytecode() { disassembler->disassemble(co); }

private:
  /**
   * Enters a new scope.
   */
  void scopeEnter() { co->scopeLevel++; }

  /**
   * Returns to previous scope level
   */
  void scopeExit() {
    // Pops vars from the stack if they were declared
    // within this specific scope.
    auto varsCount = getVarsCountOnScopeExit();

    if (varsCount > 0) {
      emit(OP_SCOPE_EXIT);
      emit(varsCount);
    }

    co->scopeLevel--;
  }

  /**
   * Checks whether it is the global scope.
   */
  bool isGlobalScope() { return co->name == "main" && co->scopeLevel == 1; }

  /**
   * Checkes whether the expression is a declaration
   */
  bool isDeclaration(const Exp &exp) { return isVarDeclaration(exp); }

  /**
   * (var <name> <value>)
   */
  bool isVarDeclaration(const Exp &exp) { return isTaggedList(exp, "var"); }

  /**
   * Tagged lists.
   */
  bool isTaggedList(const Exp &exp, const std::string &tag) {
    return exp.type == ExpType::LIST && exp.list[0].type == ExpType::SYMBOL &&
           exp.list[0].string == tag;
  }

  /**
   * Number of local vars in this scope.
   */
  size_t getVarsCountOnScopeExit() {
    auto varsCount = 0;

    if (co->locals.size() > 0) {
      while (co->locals.back().scope == co->scopeLevel) {
        co->locals.pop_back();
        varsCount++;
      }
    }

    return varsCount;
  }

  /**
   * Returns current bytecode offset.
   */
  size_t getOffset() { return co->code.size(); }

  /**
   * Allocates a numeric constant.
   */
  size_t numericConstIdx(double value) {
    ALLOC_CONST(IS_NUMBER, AS_NUMBER, NUMBER, value);
    return co->constants.size() - 1;
  }

  /**
   * Allocates a boolean constant.
   */
  size_t booleanConstIdx(bool value) {
    ALLOC_CONST(IS_BOOLEAN, AS_BOOLEAN, BOOLEAN, value);
    return co->constants.size() - 1;
  }

  /**
   * Allocates a string constant.
   */
  size_t stringConstIdx(const std::string &value) {
    ALLOC_CONST(IS_STRING, AS_CPPSTRING, ALLOC_STRING, value);
    return co->constants.size() - 1;
  }

  /**
   * Emits data to the bytecode.
   */
  void emit(uint8_t code) { co->code.push_back(code); }

  /**
   * Writes byte at offset
   */
  void writeByteAtOffset(size_t offset, uint8_t value) {
    co->code[offset] = value;
  }

  /**
   * Patches jump address.
   */
  void patchJumpAddress(size_t offset, uint16_t value) {
    writeByteAtOffset(offset, (value >> 8) & 0xff);
    writeByteAtOffset(offset + 1, value & 0xff);
  }

  /**
   * Global object.
   */
  std::shared_ptr<Global> global;

  /**
   * Disassembler.
   */
  std::unique_ptr<JellyDisassembler> disassembler;

  /**
   * Compiling code object.
   */
  CodeObject *co;

  /**
   * Compare ops map.
   */
  static std::map<std::string, uint8_t> compareOps_;
};

/**
 * Compare ops map.
 */
std::map<std::string, u_int8_t> JellyCompiler::compareOps_ = {
    {"<", 0}, {">", 1}, {"==", 2}, {"<=", 3}, {">=", 4}, {"!=", 5},
};

#endif