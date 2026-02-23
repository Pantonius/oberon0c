#include <catch2/catch_test_macros.hpp>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <memory>

inline std::unique_ptr<llvm::ExecutionEngine>
create_jit(std::unique_ptr<llvm::Module> module) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  std::string err;
  llvm::EngineBuilder engine_builder(std::move(module));
  engine_builder.setErrorStr(&err);
  engine_builder.setVerifyModules(true);
  engine_builder.setEngineKind(llvm::EngineKind::JIT);

  std::unique_ptr<llvm::ExecutionEngine> exec_engine(engine_builder.create());

  if (!exec_engine) {
    FAIL("Could not create ExecutionEngine: \n  " + err);
  }

  return exec_engine;
}
