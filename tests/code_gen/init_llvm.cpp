#include "codegen/CodeGen.h"
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <memory>

using std::string;

CodeGenBuilder init_code_gen_builder(Logger &logger) {

  static auto ctx = std::make_unique<llvm::LLVMContext>();
  static auto module = std::make_unique<llvm::Module>("test", *ctx);

  return CodeGenBuilder(logger, *module);
}
