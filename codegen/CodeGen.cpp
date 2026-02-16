#include "CodeGen.h"
#include "global.h"
#include <iostream>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>

std::unique_ptr<llvm::TargetMachine> CodeGen::init() {
  // initialize LLVM
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  // use default target triple of host
  const string triple_ = llvm::sys::getDefaultTargetTriple();
  const auto triple = llvm::Triple(triple_);

  // set up target
  string error;
  if (const auto target = llvm::TargetRegistry::lookupTarget(triple, error);
      !target) {
    logger_.error(EMPTY_POS, error);
  } else {
    // set up target machine to match host
    const string cpu = "generic";
    const string features;
    const llvm::TargetOptions opt;
    constexpr auto model = std::optional<llvm::Reloc::Model>();

    return std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(triple, cpu, features, opt, model));
  }
  return nullptr;
}

void CodeGen::emit(std::unique_ptr<llvm::TargetMachine> tm,
                   std::unique_ptr<llvm::Module> module, const string &name,
                   const OutputFileType type) {
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
    logger_.error(EMPTY_POS, ec.message());
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
    logger_.error(EMPTY_POS,
                  "Error: target machine cannot emit a file of this type.");
    return;
  }
  pass.run(*module);
  output.flush();
}

void CodeGen::build(ASTContext &ast_ctx, string name) {
  if (auto tm = init()) {
    // set up LLVM module
    auto ctx = std::make_unique<llvm::LLVMContext>();
    auto module = std::make_unique<llvm::Module>(name, *ctx);

    module->setDataLayout(tm->createDataLayout());
    module->setTargetTriple(tm->getTargetTriple());

    // set up a builder to generate the LLVM intermediate representation
    auto builder = CodeGenBuilder(logger_, *module);
    builder.build(ast_ctx);

    // verify the module
    llvm::verifyModule(*module, &llvm::errs());

    emit(std::move(tm), std::move(module), name,
         OutputFileType::LLVMIRFile); // TODO OutputFileType may be an argument

    return;
  }
  logger_.error(EMPTY_POS, "LLVM TargetMachine could not be intialized.");
  exit(EXIT_FAILURE); // TODO may be an exception
}

void CodeGen::test_unique_ptr(std::unique_ptr<llvm::TargetMachine> tm) {
  tm->getTargetTriple();
}

void CodeGenBuilder::build(ASTContext &ctx) { ctx.get_module()->accept(*this); }

void CodeGenBuilder::visit(ArrayTypeNode &array_type) {
  auto type = llvm::ArrayType::get(
      getLLVMType(array_type.type),
      array_type.expression->value); // TODO check signedness
  types_[&array_type] = type;
}

TypeNode *CodeGenBuilder::traverse_selectors(
    const DeclarationNode *ref,
    const vector<unique_ptr<SelectorNode>>::iterator start,
    const vector<unique_ptr<SelectorNode>>::iterator end) {

  TypeNode *curr_type = ref->type;
  for (auto it = start; it != end; it++) {
    const auto sel = it->get();
    if (sel->getNodeType() == NodeType::array_selector) {
      auto array_type = dynamic_cast<ArrayTypeNode *>(curr_type);

      curr_type = array_type->type;
    } else if (sel->getNodeType() == NodeType::record_selector) {
    }
  }
}

void CodeGenBuilder::visit(AssignmentNode &assignment) {
  auto ltype = assignment.ref->type;
  auto rtype = assignment.expression->type;

  llvm::Value *lvalue;
  if (assignment.ident_expr->selectors.size() > 0) {

  } else {
    llvm::Value *lvalue = values_[assignment.ref];
  }
  assignment.expression->accept(*this);
  llvm::Value *rvalue = value_;

  if (rtype->getNodeType() == NodeType::array_type) {
    // ARRAY
    // copy that array into lhs
    auto larray = dynamic_cast<const ArrayTypeNode *>(ltype);
    auto rarray = dynamic_cast<const ArrayTypeNode *>(rtype);

    // the copy length is the smaller of the two (don't want to copy dead space)
    auto copy_length =
        std::min(larray->expression->value, rarray->expression->value);

    auto layout = module_.getDataLayout();
    // needed allocation size for given llvm type
    auto elem_size = layout.getTypeAllocSize(getLLVMType(larray->type));

    auto size = builder_->getInt64(copy_length * elem_size);

    value_ = builder_->CreateMemCpy(lvalue, {}, rvalue, {}, size);
  } else if (rtype->getNodeType() == NodeType::record_type) {
    // RECORD
    auto layout = module_.getDataLayout();
    // needed size is the entire rhs
    auto size = builder_->getInt64(layout.getTypeAllocSize(getLLVMType(rtype)));

    // copy that record into lhs
    value_ = builder_->CreateMemCpy(lvalue, {}, rvalue, {}, size);
  } else {
    // straight up variable assignment to value: i := 4
    value_ = builder_->CreateStore(rvalue, lvalue);
  }
}
void CodeGenBuilder::visit(IfStatementNode &if_stmt) {}
void CodeGenBuilder::visit(ElsIfStatementNode &elsif) {}
void CodeGenBuilder::visit(ModuleNode &module_node) {
  module_.setModuleIdentifier(module_node.ident->value);

  for (auto &type : *module_node.get_types()) {
    // NOTE TypeDeclarations are simply new key:value pairs in types_
    // differentiation of semantically different types is gone now;
    // the parser / sema checker did that
    getLLVMType(type->type);
  }

  for (auto &var : *module_node.get_vars()) {
    auto type = getLLVMType(var->type);
    auto value = new llvm::GlobalVariable(
        module_, type, false, llvm::GlobalValue::InternalLinkage,
        llvm::Constant::getNullValue(type), var->ident->value);

    values_[var.get()] = value;
  }

  for (auto &proc : *module_node.get_procs()) {
    proc->accept(*this);
  }

  auto main = module_.getOrInsertFunction("main", builder_->getInt32Ty());
  const auto function = cast<llvm::Function>(main.getCallee());
  const auto entry =
      llvm::BasicBlock::Create(builder_->getContext(), "entry", function);
  builder_->SetInsertPoint(entry);

  // statements
  if (module_node.get_statements() &&
      module_node.get_statements()->stmts.size() > 0) {
    module_node.get_statements()->accept(*this);
  }

  builder_->CreateRet(builder_->getInt32(0));
}
void CodeGenBuilder::visit(ConstDeclarationNode &) {}
void CodeGenBuilder::visit(VarDeclarationNode &) {}
void CodeGenBuilder::visit(TypeDeclarationNode &) {}
void CodeGenBuilder::visit(ParamDeclarationNode &) {}

void CodeGenBuilder::visit(ProcedureCallNode &procedure_call) {}
void CodeGenBuilder::visit(ProcedureDeclarationNode &proc) {
  auto proc_type = dynamic_cast<ProcedureTypeNode *>(proc.type);

  if (proc.get_procs()->size() > 0) {
    logger_.error(proc.pos(), "CodeGen found nested procs (unsupported).");
  }

  // introduce types
  for (size_t i = 0; i < proc.get_types()->size(); i++) {
    proc.get_types()->at(i)->accept(*this);
  }

  vector<llvm::Type *> param_types;
  for (auto &param : proc_type->formal_parameters) {
    auto param_type = getLLVMType(param->type);
    param_types.push_back(
        param->by_reference ? llvm::PointerType::get(builder_->getContext(), 0)
                            : param_type);
  }
  // NOTE as of now, there are no return types; all procedures return void
  auto fun_type =
      llvm::FunctionType::get(builder_->getVoidTy(), param_types, false);

  auto callee = module_.getOrInsertFunction(proc.ident->value, fun_type);
  const auto func = cast<llvm::Function>(callee.getCallee());
  const auto entry =
      llvm::BasicBlock::Create(builder_->getContext(), "entry", func);
  builder_->SetInsertPoint(entry);

  const auto layout = module_.getDataLayout();

  // allocate space for params
  llvm::Function::arg_iterator curr_arg = func->arg_begin();
  for (auto &param : proc_type->formal_parameters) {
    auto *param_type =
        param->by_reference ? builder_->getPtrTy() : getLLVMType(param->type);
    auto *alloc =
        builder_->CreateAlloca(param_type, nullptr, param->ident->value);
    alloc->setAlignment(layout.getABITypeAlign(param_type));

    // initialize memory with given value
    builder_->CreateStore(curr_arg, alloc);
    // associate param with memory area for any expressions that need the
    // latest declared value
    values_[param.get()] = alloc;

    curr_arg++;
  }

  // allocate space for vars
  for (size_t i = 0; i < proc.get_vars()->size(); i++) {
    const auto &var = proc.get_vars()->at(i);
    llvm::Type *type = getLLVMType(var->type);

    auto *alloc = builder_->CreateAlloca(type, nullptr, var->ident->value);
    alloc->setAlignment(layout.getABITypeAlign(type));

    // associate var with memory area for any exprs that need latest value
    values_[var.get()] = alloc;
  }

  // TODO body statements

  builder_->CreateRetVoid();
  llvm::verifyFunction(*func, &llvm::errs());
}
void CodeGenBuilder::visit(IdentExpressionNode &) {}
void CodeGenBuilder::visit(BinaryExpressionNode &binary_expr) {
  const auto left_type = binary_expr.left_expression->type;
  const auto right_type = binary_expr.right_expression->type;

  binary_expr.left_expression->accept(*this);
  const auto left_value = value_;
  binary_expr.right_expression->accept(*this);
  const auto right_value = value_;

  // TODO
  if (left_type == ASTContext::INTEGER.get() &&
      right_type == ASTContext::INTEGER.get()) {
    switch (binary_expr.op) {
    case BinaryOpType::plus:
      value_ = builder_->CreateAdd(left_value, right_value);
      break;
    case BinaryOpType::minus:
      value_ = builder_->CreateSub(left_value, right_value);
      break;
    case BinaryOpType::times:
      value_ = builder_->CreateMul(left_value, right_value);
      break;
    case BinaryOpType::divide:
    case BinaryOpType::div:
      value_ = builder_->CreateSDiv(left_value, right_value);
      break;
    case BinaryOpType::mod:
      // a mod n = a - n * floor(a/n)
      // floor(x) = fptosi(x)
      value_ = builder_->CreateSDiv(left_value, right_value);
      value_ = builder_->CreateFPToSI(value_, left_value->getType());
      value_ = builder_->CreateMul(value_, right_value);
      value_ = builder_->CreateSub(value_, left_value);
      break;
    case BinaryOpType::eq:
      value_ = builder_->CreateICmpEQ(left_value, right_value);
      break;
    case BinaryOpType::neq:
      value_ = builder_->CreateICmpNE(left_value, right_value);
      break;
    case BinaryOpType::lt:
      value_ = builder_->CreateICmpSLT(left_value, right_value);
      break;
    case BinaryOpType::leq:
      value_ = builder_->CreateICmpSLE(left_value, right_value);
      break;
    case BinaryOpType::gt:
      value_ = builder_->CreateICmpSGT(left_value, right_value);
      break;
    case BinaryOpType::geq:
      value_ = builder_->CreateICmpSGE(left_value, right_value);
      break;
    default:
      logger_.error(binary_expr.pos(), "UNKNOWN OPERATOR");
      exit(EXIT_FAILURE);
    }
  } else if (left_type == ASTContext::BOOLEAN.get() &&
             right_type == ASTContext::BOOLEAN.get()) {
    switch (binary_expr.op) {
    case BinaryOpType::b_and:
      break;
    case BinaryOpType::b_or:
      break;
    default:
      logger_.error(binary_expr.pos(), "UNKNOWN OPERATOR");
      exit(EXIT_FAILURE);
    }
  }
}
void CodeGenBuilder::visit(UnaryExpressionNode &) {}
void CodeGenBuilder::visit(NumberExpressionNode &number) {
  value_ = value_ = llvm::ConstantInt::getSigned(
      builder_->getInt32Ty(), static_cast<int32_t>(number.value));
}
void CodeGenBuilder::visit(BooleanExpressionNode &) {}
void CodeGenBuilder::visit(ProcedureTypeNode &) {
} // TODO probably not of interest for now
void CodeGenBuilder::visit(RecordTypeNode &record_type) {

  vector<llvm::Type *> field_types;
  for (auto &field : record_type.field_lists) {
    field_types.push_back(getLLVMType(field->type));
  }
  auto struct_type = llvm::StructType::get(builder_->getContext(), field_types);
  types_[&record_type] = struct_type;
}
void CodeGenBuilder::visit(RepeatStatementNode &repeat_statement) {}
void CodeGenBuilder::visit(SelectorNode &selector) {}
void CodeGenBuilder::visit(StatementSequenceNode &stmts) {
  for (auto &stmt : stmts.stmts) {
    stmt->accept(*this);
  }
}
void CodeGenBuilder::visit(IdentNode &ident) {}
void CodeGenBuilder::visit(IdentTypeNode &ident) {}
void CodeGenBuilder::visit(FieldNode &field) {}
void CodeGenBuilder::visit(WhileStatementNode &while_statement) {}

llvm::Type *CodeGenBuilder::getLLVMType(TypeNode *type) {
  if (!type) {
    logger_.error(EMPTY_POS, "Encountered nullptr type in CodeGen.");
    exit(EXIT_FAILURE);
  } else if (types_[type]) {
    return types_[type];
  } else if (type == ASTContext::INTEGER.get()) {
    return builder_->getInt64Ty();
  } else if (type == ASTContext::BOOLEAN.get()) {
    return builder_->getInt1Ty();
  }

  type->accept(*this);
  return types_[type];
}
