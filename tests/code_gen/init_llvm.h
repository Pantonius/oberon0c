#include "codegen/CodeGen.h"
#include <llvm/Target/TargetMachine.h>
llvm::TargetMachine *init_llvm_tm();

CodeGenBuilder init_code_gen_builder(Logger &logger);
