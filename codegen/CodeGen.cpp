#include "CodeGen.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

inline llvm::TargetMachine *init() {
  // initialize LLVM
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  // use default target triple of host
  const string triple = llvm::sys::getDefaultTargetTriple();
  // set up target
  string error;
  if (const auto target = llvm::TargetRegistry::lookupTarget(triple, error);
      !target) {
    std::cerr << error << std::endl;
  } else {
    // set up target machine to match host
    const string cpu = "generic";
    const string features;
    const llvm::TargetOptions opt;
    constexpr auto model = std::optional<llvm::Reloc::Model>();
    return target->createTargetMachine(llvm::Triple(triple), cpu, features, opt,
                                       model);
  }
  return nullptr;
}

inline void emit(llvm::TargetMachine *tm, llvm::Module *module,
                 const string &name, const OutputFileType type) {
  string ext;
  switch (type) {
  case OutputFileType::AssemblyFile:
    ext = ".s";
    break;
  case OutputFileType::LLVMIRFile:
    ext = ".ll";
    break;
  default:
#if defined(_WIN32) || defined(_WIN64)
    ext = ".obj";
#else
    ext = ".o";
#endif
    break;
  }
  const string file = name + ext;

  // serialize LLVM module to file
  std::error_code ec;
  // open an output stream with open flags "None"
  llvm::raw_fd_ostream output(file, ec, llvm::sys::fs::OF_None);
  if (ec) {
    std::cerr << ec.message() << std::endl;
    exit(ec.value());
  }
  if (type == OutputFileType::LLVMIRFile) {
    module->print(output, nullptr);
    output.flush();
    return;
  }
  llvm::CodeGenFileType ft;
  switch (type) {
  case OutputFileType::AssemblyFile:
    ft = llvm::CodeGenFileType::AssemblyFile;
    break;
  default:
    ft = llvm::CodeGenFileType::ObjectFile;
    break;
  }
  llvm::legacy::PassManager pass;
  if (tm->addPassesToEmitFile(pass, output, nullptr, ft)) {
    std::cerr << "Error: target machine cannot emit a file of this type."
              << std::endl;
    return;
  }
  pass.run(*module);
  output.flush();
}

void CodeGen::build(ASTContext &ast_ctx, string name) {
  if (const auto tm = init()) {
    // set up LLVM module
    llvm::LLVMContext ctx;
    const auto module = new llvm::Module(name, ctx);
    module->setDataLayout(tm->createDataLayout());
    module->setTargetTriple(tm->getTargetTriple());

    // set up a builder to generate the LLVM intermediate representation
    auto builder = CodeGenBuilder(ctx);

    // verify the module
    verifyModule(*module, &llvm::errs());

    emit(tm, module, name,
         OutputFileType::LLVMIRFile); // TODO OutputFileType may be an argument
  }
  exit(EXIT_FAILURE); // TODO may be an exception
}

void CodeGenBuilder::build(ASTContext &ctx) { ctx.get_module()->accept(*this); }
void CodeGenBuilder::visit(ArrayTypeNode &array_type) {}
void CodeGenBuilder::visit(AssignmentNode &assignment) {}
void CodeGenBuilder::visit(ConstDeclarationNode &const_declaration) {}
void CodeGenBuilder::visit(ExpressionNode &expression) {}
void CodeGenBuilder::visit(IfStatementNode &if_statement) {}
void CodeGenBuilder::visit(ModuleNode &module_node) {}
void CodeGenBuilder::visit(ProcedureCallNode &procedure_call) {}
void CodeGenBuilder::visit(ProcedureDeclarationNode &procedure_declaration) {}
void CodeGenBuilder::visit(RecordTypeNode &record_type) {}
void CodeGenBuilder::visit(RepeatStatementNode &repeat_statement) {}
void CodeGenBuilder::visit(SelectorNode &selector) {}
void CodeGenBuilder::visit(StatementNode &statement) {}
void CodeGenBuilder::visit(StatementSequenceNode &statement_sequence) {}
void CodeGenBuilder::visit(IdentNode &ident) {}
void CodeGenBuilder::visit(FieldNode &field) {}
void CodeGenBuilder::visit(TypeNode &type) {}
void CodeGenBuilder::visit(TypeDeclarationNode &type_declaration) {}
void CodeGenBuilder::visit(ParamDeclarationNode &param_declaration) {}
void CodeGenBuilder::visit(VarDeclarationNode &var_declaration) {}
void CodeGenBuilder::visit(WhileStatementNode &while_statement) {}
