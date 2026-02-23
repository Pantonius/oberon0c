#include "CodeGen.h"
#include "global.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/TypeNode.h"
#include <cstdlib>
#include <iostream>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
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
#include <llvm/Support/Casting.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>
#include <stdexcept>

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

void CodeGenBuilder::build(ASTContext &ctx) {
  // for (auto &std_type : ASTContext::std_types) {
  //   std_type.second->accept(*this);
  // }

  ctx.get_module()->accept(*this);
}

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
void CodeGenBuilder::visit(VarDeclarationNode &var) {
  llvm::Function *parent_func = builder_->GetInsertBlock()->getParent();
  llvm::IRBuilder<> tmp_builder(&parent_func->getEntryBlock(),
                                parent_func->getEntryBlock().begin());
  llvm::Type *llvm_type;
  try {
    llvm_type = types_.at(var.type);
  } catch (std::out_of_range &e) {
    var.type->accept(*this);
    try {
      llvm_type = types_.at(var.type);
    } catch (std::out_of_range &e) {
      logger_.debug("Error creating llvm type for " + to_string(var.type));
      exit(EXIT_FAILURE);
    }
  }
  llvm::AllocaInst *alloca =
      tmp_builder.CreateAlloca(llvm_type, nullptr, var.ident->value);
  values_.insert({&var, alloca});
  builder_->CreateStore(llvm::Constant::getNullValue(llvm_type), alloca);
}

void CodeGenBuilder::visit(TypeDeclarationNode &type) {
  type.type->accept(*this);
}

void CodeGenBuilder::visit(ParamDeclarationNode &param) {}

void CodeGenBuilder::visit(ProcedureDeclarationNode &proc) {
  auto proc_type = static_cast<ProcedureTypeNode *>(proc.type);
  proc_type->accept(*this);

  llvm::FunctionType *llvm_func_type =
      static_cast<llvm::FunctionType *>(getLLVMType(proc.type));
  auto callee = module_.getOrInsertFunction(proc.ident->value, llvm_func_type);
  const auto func = cast<llvm::Function>(callee.getCallee());

  const auto entry =
      llvm::BasicBlock::Create(builder_->getContext(), "entry", func);
  builder_->SetInsertPoint(entry);

  unsigned idx = 0;
  for (auto &arg : func->args()) {
    auto param_decl = proc_type->formal_parameters[idx++].get();
    arg.setName(param_decl->ident->value);
    values_[param_decl] = &arg;
  }

  if (proc.get_procs()->size() > 0) {
    logger_.error(proc.pos(), "CodeGen found nested procs (unsupported).");
  }

  // introduce types
  for (auto &type : *proc.get_types()) {
    type->accept(*this);
  }

  // allocate space for vars
  for (auto &var_decl : *proc.get_vars()) {
    var_decl->accept(*this);
  }

  // TODO: body statements

  builder_->CreateRetVoid();
  llvm::verifyFunction(*func, &llvm::errs());
}

void CodeGenBuilder::visit(IdentTypeNode &ident) {
  logger_.debug("Found IdentTypeNode after semantic check!");
  exit(EXIT_FAILURE);
}

void CodeGenBuilder::visit(StdTypeNode &std_type) {
  if (types_.contains(&std_type)) {
    return;
  }

  llvm::Type *type;
  llvm::Value *init_val;

  switch (std_type.std_type) {
  case StdType::BOOLEAN:
    type = builder_->getInt1Ty();
    init_val = builder_->getInt1(false);
    break;
  case StdType::INTEGER:
    type = builder_->getInt32Ty();
    init_val = builder_->getInt32(0);
    break;
  default:
    break;
  }

  types_[&std_type] = type;
}

void CodeGenBuilder::visit(ArrayTypeNode &array_type) {
  array_type.type->accept(*this);
  try {
    auto type = llvm::ArrayType::get(
        types_.at(array_type.type),
        array_type.expression->value); // TODO check signedness
    types_[&array_type] = type;
  } catch (std::out_of_range &e) {
    logger_.debug("No llvm::Type for " + to_string(array_type.type));
    exit(EXIT_FAILURE);
  }
} // TODO probably not of interest for now

void CodeGenBuilder::visit(RecordTypeNode &record_type) {

  vector<llvm::Type *> field_types;
  for (auto &field : record_type.field_lists) {
    field->type->accept(*this);
    try {
      field_types.push_back(types_.at(field->type));
    } catch (std::out_of_range &e) {
      logger_.debug("No llvm::Type for " + to_string(field->type));
      exit(EXIT_FAILURE);
    }
  }
  auto struct_type = llvm::StructType::get(builder_->getContext(), field_types);
  types_[&record_type] = struct_type;
}

void CodeGenBuilder::visit(ProcedureTypeNode &proc_type) {
  vector<llvm::Type *> param_types;
  for (auto &param : proc_type.formal_parameters) {
    auto param_llvm_type = getLLVMType(param->type);
    param_types.push_back(param->by_reference ? builder_->getPtrTy()
                                              : param_llvm_type);
  }
  auto llvm_type =
      llvm::FunctionType::get(builder_->getVoidTy(), param_types, false);

  types_[&proc_type] = llvm_type;
}

void CodeGenBuilder::visit(AssignmentNode &assignment) {
  auto ltype = assignment.ident_expr->decl->type;
  auto rtype = assignment.expression->type;

  assignment.ident_expr->accept(*this);
  llvm::Value *lvalue = value_;

  // only want a ptr value if not structured, because structured values (arrays
  // and records) are going to be copied
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

void CodeGenBuilder::visit(ProcedureCallNode &procedure_call) {}
void CodeGenBuilder::visit(IfStatementNode &if_stmt) {}
void CodeGenBuilder::visit(ElsIfStatementNode &elsif) {}

void CodeGenBuilder::visit(IdentExpressionNode &ident_expr) {
  llvm::AllocaInst *alloca;
  try {
    alloca = static_cast<llvm::AllocaInst *>(values_.at(ident_expr.decl));
  } catch (std::out_of_range &e) {
    logger_.debug("Unknown variable: " + to_string(*ident_expr.ident));
  }

  value_ = builder_->CreateLoad(alloca->getAllocatedType(), alloca, false);
}

void CodeGenBuilder::visit(BinaryExpressionNode &binary_expr) {
  const auto left_type = binary_expr.left_expression->type;
  const auto right_type = binary_expr.right_expression->type;

  binary_expr.left_expression->accept(*this);
  const auto left_value = value_;
  binary_expr.right_expression->accept(*this);
  const auto right_value = value_;

  // TODO
  if (left_type == ASTContext::INTEGER && right_type == ASTContext::INTEGER) {
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
  } else if (left_type == ASTContext::BOOLEAN &&
             right_type == ASTContext::BOOLEAN) {
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
void CodeGenBuilder::visit(UnaryExpressionNode &unary_expr) {
  unary_expr.expression->accept(*this);
  auto value = value_;
  switch (unary_expr.op) {
  case UnaryOpType::plus:
    break;
  case UnaryOpType::minus:
    value_ = builder_->CreateNeg(value);
    break;
  case UnaryOpType::u_not:
    value_ = builder_->CreateNot(value);
    break;
  default:
    logger_.error(unary_expr.pos(), "UNKNOWN OPERATOR");
    exit(EXIT_FAILURE);
  }
}
void CodeGenBuilder::visit(NumberExpressionNode &number) {
  value_ = builder_->getInt32(number.value);
}
void CodeGenBuilder::visit(BooleanExpressionNode &boolean) {
  value_ = builder_->getInt1(boolean.value);
}
void CodeGenBuilder::visit(RepeatStatementNode &repeat_statement) {}
void CodeGenBuilder::visit(SelectorNode &selector) {}
void CodeGenBuilder::visit(StatementSequenceNode &stmts) {
  for (auto &stmt : stmts.stmts) {
    stmt->accept(*this);
  }
}
void CodeGenBuilder::visit(IdentNode &ident) {}
void CodeGenBuilder::visit(FieldNode &field) {}
void CodeGenBuilder::visit(WhileStatementNode &while_statement) {}

llvm::Type *CodeGenBuilder::getLLVMType(TypeNode *const type) {
  llvm::Type *llvm_type;
  try {
    llvm_type = types_.at(type);
  } catch (std::out_of_range &e) {
    logger_.debug("No llvm type found for " + to_string(type));
    exit(EXIT_FAILURE);
  }

  return llvm_type;
}

TypeNode *CodeGenBuilder::traverse_selectors(
    const DeclarationNode *ref,
    const vector<unique_ptr<SelectorNode>>::iterator start,
    const vector<unique_ptr<SelectorNode>>::iterator end) {

  TypeNode *curr_type = ref->type;
  llvm::Value *value = value_;

  for (auto it = start; it != end; it++) {
    const auto sel = it->get();
    if (sel->getNodeType() == NodeType::array_selector) {
      auto array_index = dynamic_cast<ArrayIndexNode *>(sel);
      auto array_type = dynamic_cast<ArrayTypeNode *>(curr_type);

      // visit index expression
      array_index->expression->accept(*this);

      // value_ is now the llvm::Value of the index expression
      value =
          builder_->CreateInBoundsGEP(getLLVMType(curr_type), value, {value_});
      curr_type = array_type->type;
    } else if (sel->getNodeType() == NodeType::record_selector) {
      auto record_selector = dynamic_cast<RecordFieldNode *>(sel);
      auto record_type = dynamic_cast<RecordTypeNode *>(curr_type);

      // find the field index referred to by the selector ident
      auto field_index = record_type->find_field_index(*record_selector->ident);

      value = builder_->CreateInBoundsGEP(getLLVMType(curr_type), value,
                                          {builder_->getInt32(field_index)});
      curr_type = record_type->field_lists.at(field_index)->type;
    }
  }

  value_ = value;

  return curr_type;
}

void CodeGenBuilder::init_values(llvm::Value *ptr, llvm::Type *llvm_type) {
  builder_->CreateStore(llvm::Constant::getNullValue(llvm_type), ptr);

  // if (llvm_type->isIntegerTy(1)) {
  //   builder_->CreateStore(builder_->getInt1(false), ptr);
  //   return;
  // }
  //
  // if (llvm_type->isIntegerTy(32)) {
  //   builder_->CreateStore(builder_->getInt32(0), ptr);
  //   return;
  // }
  //
  // if (auto llvm_array_type = llvm::dyn_cast<llvm::ArrayType>(llvm_type)) {
  //   for (unsigned i = 0; i <= llvm_array_type->getNumElements(); i++) {
  //     auto elem_ptr =
  //         builder_->CreateConstInBoundsGEP1_32(llvm_array_type, ptr, i);
  //     init_values(elem_ptr, llvm_array_type->getElementType());
  //   }
  //   return;
  // }
  //
  // if (auto llvm_record_type = llvm::dyn_cast<llvm::StructType>(llvm_type)) {
  //   for (unsigned i = 0; i <= llvm_record_type->getNumElements(); ++i) {
  //     auto elem_ptr = builder_->CreateStructGEP(llvm_record_type, ptr, i);
  //     init_values(elem_ptr, llvm_record_type->getElementType(i));
  //   }
  //   return;
  // }
  //
  // logger_.debug("Unknown llvm type: ");
  // llvm_type->print(llvm::errs());
}

void CodeGenBuilder::pushRefCtx(const bool ref_status) {
  ref_ctx_.push(ref_status);
}

void CodeGenBuilder::popRefCtx() { return ref_ctx_.pop(); }
bool CodeGenBuilder::peekRefCtx() const {
  return ref_ctx_.empty() ? false : ref_ctx_.top();
}
