#include "CodeGen.h"
#include "global.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/StatementNode.h"
#include "parser/ast/TypeNode.h"
#include <cstdlib>
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
#include <llvm/IR/Value.h>
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
#include <unordered_map>

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

llvm::FunctionCallee CodeGenBuilder::getPrintf() {
  auto printf_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(builder_->getContext()),
      {llvm::PointerType::get(builder_->getContext(), 0)}, true);
  return module_.getOrInsertFunction("printf", printf_type);
}

llvm::GlobalVariable *CodeGenBuilder::getIntFmt() {
  auto fmtStr =
      llvm::ConstantDataArray::getString(builder_->getContext(), "%d", true);

  if (llvm::GlobalVariable *fmt = module_.getGlobalVariable("intFmt"))
    return fmt;

  return new llvm::GlobalVariable(module_, fmtStr->getType(), true,
                                  llvm::GlobalValue::PrivateLinkage, fmtStr,
                                  "intFmt");
}

void CodeGenBuilder::generateWriteInt() {
  auto printf_callee = getPrintf();
  auto fmt = getIntFmt();

  auto write_int_type =
      static_cast<ProcedureTypeNode *>(ASTContext::WRITE_INT->type);
  write_int_type->accept(*this);

  llvm::FunctionType *llvm_func_type =
      static_cast<llvm::FunctionType *>(getLLVMType(write_int_type));
  auto callee = module_.getOrInsertFunction(ASTContext::WRITE_INT->ident->value,
                                            llvm_func_type);
  const auto func = cast<llvm::Function>(callee.getCallee());

  const auto entry =
      llvm::BasicBlock::Create(builder_->getContext(), "entry", func);
  builder_->SetInsertPoint(entry);

  auto val = builder_->CreateAlloca(builder_->getInt32Ty(), nullptr, "val");

  auto arg_val = func->arg_begin();
  builder_->CreateStore(arg_val, val);

  auto load_val = builder_->CreateLoad(builder_->getInt32Ty(), val);

  builder_->CreateCall(printf_callee, {fmt, load_val});

  builder_->CreateRetVoid();
  llvm::verifyFunction(*func, &llvm::errs());
}

void CodeGenBuilder::generateWriteLn() {
  auto printf_callee = getPrintf();

  auto write_ln_type =
      static_cast<ProcedureTypeNode *>(ASTContext::WRITE_LN->type);
  write_ln_type->accept(*this);

  llvm::FunctionType *llvm_func_type =
      static_cast<llvm::FunctionType *>(getLLVMType(write_ln_type));
  auto callee = module_.getOrInsertFunction(ASTContext::WRITE_LN->ident->value,
                                            llvm_func_type);
  const auto func = cast<llvm::Function>(callee.getCallee());

  const auto entry =
      llvm::BasicBlock::Create(builder_->getContext(), "entry", func);
  builder_->SetInsertPoint(entry);

  llvm::Constant *nl_val =
      llvm::ConstantDataArray::getString(builder_->getContext(), "\n", true);

  auto nl = builder_->CreateAlloca(builder_->getInt32Ty(), nullptr, "nl");
  builder_->CreateStore(nl_val, nl);

  builder_->CreateCall(printf_callee, {nl});

  builder_->CreateRetVoid();
  llvm::verifyFunction(*func, &llvm::errs());
}

void CodeGenBuilder::build(ASTContext &ctx) {
  for (auto &type : ctx.std_types) {
    type.second->accept(*this);
  }

  generateWriteInt();
  generateWriteLn();

  ctx.get_module()->accept(*this);
}

void CodeGenBuilder::visit(IfStatementNode &if_stmt) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  if_stmt.condition->accept(*this);
  auto condition = value_;

  auto tailBlock =
      llvm::BasicBlock::Create(builder_->getContext(), "ifTail", currentFunc);
  auto trueBlock =
      llvm::BasicBlock::Create(builder_->getContext(), "ifTrue", currentFunc);
  auto falseBlock =
      llvm::BasicBlock::Create(builder_->getContext(), "ifFalse", currentFunc);

  // [
  return_points_.push(tailBlock);
  builder_->CreateCondBr(condition, trueBlock, falseBlock);

  builder_->SetInsertPoint(trueBlock);
  if_stmt.body->accept(*this);
  builder_->CreateBr(tailBlock);

  builder_->SetInsertPoint(falseBlock);
  for (auto &elsif : if_stmt.elsifs) {
    elsif->accept(*this);
  }
  if (if_stmt.else_statement_sequence) {
    if_stmt.else_statement_sequence->accept(*this);
  }
  builder_->CreateBr(tailBlock);
  return_points_.pop();
  // ]

  builder_->SetInsertPoint(tailBlock);
}
void CodeGenBuilder::visit(ElsIfStatementNode &elsif) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  elsif.condition->accept(*this);
  auto condition = value_;

  auto trueBlock = llvm::BasicBlock::Create(builder_->getContext(), "elsifTrue",
                                            currentFunc);
  auto falseBlock = llvm::BasicBlock::Create(builder_->getContext(),
                                             "elsifFalse", currentFunc);

  builder_->CreateCondBr(condition, trueBlock, falseBlock);

  builder_->SetInsertPoint(trueBlock);
  elsif.body->accept(*this);
  builder_->CreateBr(return_points_.top());

  builder_->SetInsertPoint(falseBlock);
}

void CodeGenBuilder::visit(ModuleNode &module_node) {
  module_.setModuleIdentifier(module_node.ident->value);

  for (auto &type : *module_node.get_types()) {
    type->accept(*this);
    getLLVMType(type->type);
  }

  for (auto &var : *module_node.get_vars()) {
    var->type->accept(*this);
    auto type = getLLVMType(var->type);
    auto value = new llvm::GlobalVariable(
        module_, type, false, llvm::GlobalValue::ExternalLinkage,
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

void CodeGenBuilder::visit(ProcedureCallNode &procedure_call) {
  if (procedure_call.selectors.size() > 0) {
    logger_.error(procedure_call.pos(),
                  "Cannot handle procedure calls with selectors as of yet.");
    exit(EXIT_FAILURE);
  }

  auto callee = module_.getFunction(procedure_call.ident->value);

  vector<llvm::Value *> actual_values;
  for (auto &param : procedure_call.actual_parameters) {
    param->accept(*this);
    actual_values.push_back(value_);
  }

  builder_->CreateCall(callee, actual_values);
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
  init_values(alloca, llvm_type);
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

  if (proc.get_statements()) {
    logger_.warning(
        proc.pos(),
        "Found procedure declaration without statements in its body.");
    proc.get_statements()->accept(*this);
  }

  builder_->CreateRetVoid();
  llvm::verifyFunction(*func, &llvm::errs());
}

void CodeGenBuilder::visit(IdentTypeNode &) {
  logger_.debug("Found IdentTypeNode after semantic check!");
  exit(EXIT_FAILURE);
}

void CodeGenBuilder::visit(StdTypeNode &std_type) {
  if (types_.contains(&std_type)) {
    return;
  }

  llvm::Type *type;

  switch (std_type.std_type) {
  case StdType::BOOLEAN:
    type = builder_->getInt1Ty();
    break;
  case StdType::INTEGER:
    type = builder_->getInt32Ty();
    break;
  default:
    break;
  }

  types_[&std_type] = type;
}

void CodeGenBuilder::visit(ArrayTypeNode &array_type) {
  array_type.type->accept(*this);
  auto type = llvm::ArrayType::get(
      getLLVMType(array_type.type),
      array_type.expression->value); // TODO check signedness
  types_[&array_type] = type;
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

void CodeGenBuilder::visit(SumTypeNode &sum_type) {
  // variant payload array
  auto max_size = llvm::TypeSize::getZero();

  for (auto &variant : sum_type.variants) {
    vector<llvm::Type *> param_types;
    for (auto &param : variant->parameter_types->formal_parameters) {
      param->type->accept(*this);
      param_types.push_back(getLLVMType(param->type));
    }
    // NOTE had to be a llvm::StrucType instead of just visiting the
    // ProcedureTypeNode (yielding a FunctionType), because FunctionType is not
    // sized
    auto variant_type =
        llvm::StructType::get(builder_->getContext(), param_types);
    auto variant_size = module_.getDataLayout().getTypeAllocSize(variant_type);

    types_[variant->parameter_types] = variant_type;

    if (max_size < variant_size) {
      max_size = variant_size;
    }
  }

  // sum type consists of tag (Int32) and variant payload array (max size of all
  // variants)
  auto payload_type = llvm::ArrayType::get(builder_->getInt8Ty(), max_size);
  auto llvm_type = llvm::StructType::get(
      builder_->getContext(), {builder_->getInt32Ty(), payload_type});

  types_[&sum_type] = llvm_type;
};

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

void CodeGenBuilder::visit(AssignmentNode &assign) {
  auto ltype = assign.ident_expr->decl->type;
  auto rtype = assign.expression->type;

  assign.ident_expr->accept(*this);
  llvm::Value *lvalue = value_;

  assign.expression->accept(*this);
  llvm::Value *rvalue = value_;

  if (rvalue->getType()->isSingleValueType() &&
      assign.expression->type->isPrimitiveType()) {
    value_ = builder_->CreateStore(rvalue, lvalue);
    return;
  }

  if (!rvalue->getType()->isPointerTy()) {
    logger_.debug(
        "rvalue of assignment is neither a primitive value nor a pointer");
    exit(EXIT_FAILURE);
  }

  auto llvm_ltype = getLLVMType(ltype);
  auto llvm_rtype = getLLVMType(rtype);

  auto lsize = module_.getDataLayout().getTypeAllocSize(llvm_ltype);
  auto rsize = module_.getDataLayout().getTypeAllocSize(llvm_rtype);

  if (lsize != rsize) {
    logger_.debug("Assignment sizes differ");
    exit(EXIT_FAILURE);
  }

  value_ = builder_->CreateMemCpy(lvalue, {}, rvalue, {}, lsize);
}

void CodeGenBuilder::visit(IdentExpressionNode &ident_expr) {
  if (ident_expr.type->getNodeType() == NodeType::sum_type &&
      !ident_expr.is_lvalue && ident_expr.selectors.size() == 1) {
    auto sum_type = dynamic_cast<const SumTypeNode *>(ident_expr.type);

    const RecordFieldNode *record_field =
        dynamic_cast<const RecordFieldNode *>(ident_expr.selectors.at(0).get());
    auto variant = sum_type->find_variant(*record_field->ident);
    auto variant_tag = sum_type->find_variant_index(*record_field->ident);

    // initialize sum type value
    try {
      auto llvm_sum_type = getLLVMType(variant->type);
      llvm::AllocaInst *dst = builder_->CreateAlloca(llvm_sum_type);

      // find variant tag and put it into the tag position
      llvm::Value *llvm_sum_tag =
          builder_->CreateConstGEP2_32(llvm_sum_type, dst, 0, 0);
      builder_->CreateStore(
          llvm::ConstantInt::get(builder_->getInt32Ty(), variant_tag),
          llvm_sum_tag);

      // build llvm_variant
      if (variant->parameter_types->formal_parameters.size() > 0) {
        try {
          auto llvm_variant_type = getLLVMType(variant->parameter_types);
          auto llvm_variant =
              builder_->CreateConstGEP2_32(llvm_sum_type, dst, 0, 1);

          for (u_int i = 0;
               i < variant->parameter_types->formal_parameters.size(); i++) {
            // visit parameter
            ident_expr.actual_parameters.at(i)->accept(*this);
            auto field_value = value_;

            auto llvm_field = builder_->CreateConstGEP2_32(llvm_variant_type,
                                                           llvm_variant, 0, i);
            builder_->CreateStore(field_value, llvm_field);
          }

          value_ = dst;
        } catch (std::out_of_range &e) {
          logger_.debug("Unknown type: " + to_string(variant->parameter_types));
          exit(EXIT_FAILURE);
        }
      }
    } catch (std::out_of_range &e) {
      logger_.debug("Unknown type: " + to_string(variant->type));
      exit(EXIT_FAILURE);
    }
  } else {
    llvm::AllocaInst *base_ptr;
    try {
      base_ptr = static_cast<llvm::AllocaInst *>(values_.at(ident_expr.decl));
    } catch (std::out_of_range &e) {
      logger_.debug("Unknown variable: " + to_string(*ident_expr.ident));
    }

    auto elem_type =
        get_elem_ptr(ident_expr.decl, base_ptr, ident_expr.selectors);

    if (elem_type->isPrimitiveType() && !ident_expr.is_lvalue) {
      value_ = builder_->CreateLoad(getLLVMType(elem_type), value_);
      return;
    }
  }
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
      value_ = builder_->CreateAnd(left_value, right_value);
      break;
    case BinaryOpType::b_or:
      value_ = builder_->CreateOr(left_value, right_value);
      break;
    case BinaryOpType::eq:
      value_ = builder_->CreateICmpEQ(left_value, right_value);
      break;
    case BinaryOpType::neq:
      value_ = builder_->CreateICmpNE(left_value, right_value);
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
void CodeGenBuilder::visit(VariantDeclarationNode &) {};
void CodeGenBuilder::visit(IdentNode &ident) {}
void CodeGenBuilder::visit(RecordFieldNode &field) {}
void CodeGenBuilder::visit(WhileStatementNode &while_stmt) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  auto tailBlock = llvm::BasicBlock::Create(builder_->getContext(), "whileTail",
                                            currentFunc);
  auto conditionBlock = llvm::BasicBlock::Create(builder_->getContext(),
                                                 "whileCondition", currentFunc);
  auto bodyBlock = llvm::BasicBlock::Create(builder_->getContext(), "whileBody",
                                            currentFunc);

  // [
  builder_->CreateBr(conditionBlock);

  builder_->SetInsertPoint(conditionBlock);
  while_stmt.condition->accept(*this);
  auto condition = value_;
  builder_->CreateCondBr(condition, bodyBlock, tailBlock);

  builder_->SetInsertPoint(bodyBlock);
  while_stmt.body->accept(*this);
  builder_->CreateBr(conditionBlock);
  // ]

  builder_->SetInsertPoint(tailBlock);
}

void CodeGenBuilder::visit(IdentPatternNode &ident_pattern) {
  auto case_value = value_;

  ident_pattern.var->accept(*this);

  llvm::AllocaInst *base_ptr;
  try {
    base_ptr =
        static_cast<llvm::AllocaInst *>(values_.at(ident_pattern.var.get()));
  } catch (std::out_of_range &e) {
    logger_.debug("Unknown variable: " + to_string(*ident_pattern.var->ident));
  }

  get_elem_ptr(ident_pattern.var.get(), base_ptr, {});

  auto pattern_value = value_;

  if (case_value->getType()->isSingleValueType() &&
      ident_pattern.type->isPrimitiveType()) {
    value_ = builder_->CreateStore(case_value, pattern_value);
    return;
  }

  if (!case_value->getType()->isPointerTy()) {
    logger_.debug(
        "rvalue of assignment is neither a primitive value nor a pointer");
    exit(EXIT_FAILURE);
  }

  auto lsize =
      module_.getDataLayout().getTypeAllocSize(pattern_value->getType());
  auto rsize = module_.getDataLayout().getTypeAllocSize(case_value->getType());

  if (lsize != rsize) {
    logger_.debug("Assignment sizes differ");
    exit(EXIT_FAILURE);
  }

  value_ = builder_->CreateMemCpy(pattern_value, {}, case_value, {}, lsize);
}

void CodeGenBuilder::visit(VariantPatternNode &variant_pattern) {}
void CodeGenBuilder::visit(NumberPatternNode &number_pattern) {
  value_ = builder_->getInt32(number_pattern.value);
}
void CodeGenBuilder::visit(BooleanPatternNode &bool_pattern) {
  value_ = builder_->getInt1(bool_pattern.value);
}

void CodeGenBuilder::literal_pattern(PatternNode *pattern,
                                     StatementSequenceNode *statements,
                                     llvm::Value *case_value) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  pattern->accept(*this);
  auto literal_value = value_;

  auto condition = builder_->CreateICmpEQ(literal_value, case_value);

  auto true_block =
      llvm::BasicBlock::Create(builder_->getContext(), "ifTrue", currentFunc);
  auto false_block =
      llvm::BasicBlock::Create(builder_->getContext(), "ifFalse", currentFunc);
  auto tail_block = return_points_.top();

  builder_->CreateCondBr(condition, true_block, false_block);
  builder_->SetInsertPoint(true_block);
  statements->accept(*this);
  builder_->CreateBr(tail_block);

  builder_->SetInsertPoint(false_block);
}

llvm::Value *
CodeGenBuilder::getCaseValueField(const VariantPatternNode *variant_pattern,
                                  size_t param_index, llvm::Value *case_value) {
  // get all information about types and variants
  auto sum_type = dynamic_cast<const SumTypeNode *>(variant_pattern->type);
  auto variant_decl = sum_type->find_variant(*variant_pattern->variant->ident);

  // construct pointer to payload
  auto case_payload_pointer = builder_->CreateConstGEP2_32(
      getLLVMType(variant_pattern->type), case_value, 0, 1);

  // construct pointer to field in payload
  auto case_curr_field_pointer =
      builder_->CreateConstGEP2_32(getLLVMType(variant_decl->parameter_types),
                                   case_payload_pointer, 0, param_index);

  // load the value at pointer
  return builder_->CreateLoad(
      getLLVMType(
          variant_decl->parameter_types->formal_parameters.at(param_index)
              ->type),
      case_curr_field_pointer);
}

template <typename T>
void CodeGenBuilder::literals(
    // case patterns are variant patterns
    vector<std::pair<PatternNode *, StatementSequenceNode *>> cases,
    // case_value is the entire value of the case stmt expression
    llvm::Value *case_value,
    const vector<unique_ptr<PatternNode>> &param_patterns, size_t param_index) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  // partition according to literal
  std::unordered_map<T,
                     vector<std::pair<PatternNode *, StatementSequenceNode *>>>
      literal_map;
  vector<std::pair<PatternNode *, StatementSequenceNode *>> wildcard_cases;

  for (auto &curr_case : cases) {
    auto variant_pattern =
        dynamic_cast<const VariantPatternNode *>(curr_case.first);
    auto curr_pattern = variant_pattern->param_patterns.at(param_index).get();

    if (curr_pattern->getNodeType() == NodeType::literal_pattern) {
      // literal pattern
      auto literal_pattern =
          dynamic_cast<const LiteralPatternNode<T> *>(curr_pattern);
      auto literal = literal_pattern->value;

      if (!literal_map.contains(literal)) {
        literal_map[literal] = {curr_case};
      } else {
        literal_map[literal].push_back(curr_case);
      }
    } else {
      // ident pattern
      wildcard_cases.push_back(curr_case);
    }
  }

  // build conditional branching for each
  for (const auto &[literal, literal_cases] : literal_map) {
    // case value param
    auto variant_pattern =
        dynamic_cast<const VariantPatternNode *>(literal_cases.begin()->first);
    auto case_value_field =
        getCaseValueField(variant_pattern, param_index, case_value);

    auto pattern_literal =
        llvm::ConstantInt::get(case_value_field->getType(), literal);

    // compare literals
    auto condition = builder_->CreateICmpEQ(pattern_literal, case_value_field);

    auto true_block =
        llvm::BasicBlock::Create(builder_->getContext(), "ifTrue", currentFunc);
    auto false_block = llvm::BasicBlock::Create(builder_->getContext(),
                                                "ifFalse", currentFunc);
    auto tail_block = return_points_.top();

    builder_->CreateCondBr(condition, true_block, false_block);
    builder_->SetInsertPoint(true_block);

    // move on to the next parameter if any
    if (param_index + 1 < param_patterns.size()) {
      vector<std::pair<PatternNode *, StatementSequenceNode *>> sub_cases;
      for (auto curr_case : literal_cases) {
        sub_cases.emplace_back(curr_case.first, curr_case.second);
      }

      auto next_param_type = param_patterns.at(param_index + 1)->type;

      if (next_param_type == ASTContext::INTEGER) {
        literals<int32_t>(sub_cases, case_value, param_patterns,
                          param_index + 1);
      } else if (next_param_type == ASTContext::BOOLEAN) {
        literals<bool>(sub_cases, case_value, param_patterns, param_index + 1);
      } else if (next_param_type->getNodeType() == NodeType::sum_type) {
        variants(sub_cases, case_value);
      } else {
        logger_.error(next_param_type->pos(), "UNEXPECTED VALUE TYPE");
        exit(EXIT_FAILURE);
      }
    } else if (literal_cases.size() == 1) {
      literal_cases.at(0).second->accept(*this);
    } else {
      // this shouldn't happen because we remove duplicates -> Update:
      // CaseSumUnreachable does produce this message right now
      logger_.debug("Overlooked duplicate?");
      // but if it does, just do the first thing (the order of equal cases is
      // preserved through-out the compiler; unless I am mistaken, the intended
      // order is maintained and any duplicate occurance can be ignored because
      // the first occurance will already have matched)
      literal_cases.at(0).second->accept(*this);
    }
    builder_->CreateBr(tail_block);

    builder_->SetInsertPoint(false_block);
  }
  for (auto &wildcard_case : wildcard_cases) {
    auto variant_pattern =
        dynamic_cast<const VariantPatternNode *>(wildcard_case.first);
    auto curr_pattern = variant_pattern->param_patterns.at(param_index).get();

    // visit ident pattern to declare variable
    value_ = getCaseValueField(variant_pattern, param_index, case_value);
    curr_pattern->accept(*this);

    // move on to the next parameter if any
    if (param_index + 1 < param_patterns.size()) {
      vector<std::pair<PatternNode *, StatementSequenceNode *>> sub_cases;
      for (auto curr_case : wildcard_cases) {
        sub_cases.emplace_back(curr_case.first, curr_case.second);
      }

      auto next_param_type = param_patterns.at(param_index + 1)->type;

      if (next_param_type == ASTContext::INTEGER) {
        literals<int32_t>(sub_cases, case_value, param_patterns,
                          param_index + 1);
      } else if (next_param_type == ASTContext::BOOLEAN) {
        literals<bool>(sub_cases, case_value, param_patterns, param_index + 1);
      } else if (next_param_type->getNodeType() == NodeType::sum_type) {
        variants(sub_cases, case_value);
      } else {
        logger_.error(next_param_type->pos(), "UNEXPECTED VALUE TYPE");
        exit(EXIT_FAILURE);
      }
    } else if (wildcard_cases.size() == 1) {
      wildcard_case.second->accept(*this);
    } else {
      // this shouldn't happen because we remove duplicates -> Update:
      // CaseSumUnreachable does produce this message right now
      logger_.debug("Overlooked duplicate?");
      // but if it does, just do the first thing (the order of equal cases is
      // preserved through-out the compiler; unless I am mistaken, the intended
      // order is maintained and any duplicate occurance can be ignored because
      // the first occurance will already have matched)
      wildcard_case.second->accept(*this);
    }
  }
}

void CodeGenBuilder::variants(
    vector<std::pair<PatternNode *, StatementSequenceNode *>> cases,
    llvm::Value *case_value) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  // partition according to variant
  std::unordered_map<string,
                     vector<std::pair<PatternNode *, StatementSequenceNode *>>>
      variant_map;
  vector<std::pair<PatternNode *, StatementSequenceNode *>> wildcard_cases;

  for (auto &curr_case : cases) {
    if (curr_case.first->getNodeType() == NodeType::variant_pattern) {
      auto variant_pattern =
          dynamic_cast<const VariantPatternNode *>(curr_case.first);
      auto variant = variant_pattern->variant->ident->value;

      if (!variant_map.contains(variant)) {
        variant_map[variant] = {curr_case};
      } else {
        variant_map[variant].push_back(curr_case);
      }
    } else {
      // ident pattern
      wildcard_cases.push_back(curr_case);
    }
  }

  for (const auto &[variant, variant_cases] : variant_map) {
    auto rep_pattern = variant_cases.begin()->first;

    auto variant_pattern =
        dynamic_cast<const VariantPatternNode *>(rep_pattern);
    auto sum_type = dynamic_cast<const SumTypeNode *>(rep_pattern->type);

    auto case_vtag_pointer = builder_->CreateConstGEP2_32(
        getLLVMType(rep_pattern->type), case_value, 0, 0);
    auto case_vtag =
        builder_->CreateLoad(builder_->getInt32Ty(), case_vtag_pointer);

    auto variant_index =
        sum_type->find_variant_index(*variant_pattern->variant->ident);
    auto pattern_vtag = builder_->getInt32(variant_index);

    // compare variant tags
    auto condition = builder_->CreateICmpEQ(pattern_vtag, case_vtag);

    auto true_block =
        llvm::BasicBlock::Create(builder_->getContext(), "ifTrue", currentFunc);
    auto false_block = llvm::BasicBlock::Create(builder_->getContext(),
                                                "ifFalse", currentFunc);
    auto tail_block = return_points_.top();

    builder_->CreateCondBr(condition, true_block, false_block);
    builder_->SetInsertPoint(true_block);

    vector<std::pair<PatternNode *, StatementSequenceNode *>> sub_cases;
    for (auto curr_case : variant_cases) {
      sub_cases.emplace_back(curr_case.first, curr_case.second);
    }

    // if there are parameters
    if (variant_pattern->param_patterns.size() > 0) {
      auto curr_param_type = variant_pattern->param_patterns.at(0)->type;

      // generate conditional branching according to first parameter type
      if (curr_param_type == ASTContext::INTEGER) {
        literals<int32_t>(sub_cases, case_value,
                          variant_pattern->param_patterns, 0);
      } else if (curr_param_type == ASTContext::BOOLEAN) {
        literals<bool>(sub_cases, case_value, variant_pattern->param_patterns,
                       0);
      } else if (curr_param_type->getNodeType() == NodeType::sum_type) {
        logger_.error(curr_param_type->pos(),
                      "Can't generate code for sum types in sum types :(");
        exit(EXIT_FAILURE);
        // variants(sub_cases, case_value); // TODO
      } else {
        logger_.error(curr_param_type->pos(), "UNEXPECTED VALUE TYPE");
        exit(EXIT_FAILURE);
      }
      // if there are no parameters, the following may hold
    } else if (variant_cases.size() == 1) {
      // visit statement sequence
      variant_cases.at(0).second->accept(*this);
    } else {
      // this shouldn't happen because we remove duplicates -> Update:
      // CaseSumUnreachable does produce this message right now
      logger_.debug("Overlooked duplicate?");
      // but if it does, just do the first thing (the order of equal cases is
      // preserved through-out the compiler; unless I am mistaken, the intended
      // order is maintained and any duplicate occurance can be ignored because
      // the first occurance will already have matched)
      variant_cases.at(0).second->accept(*this);
    }
    builder_->CreateBr(tail_block);

    builder_->SetInsertPoint(false_block);
  }
  for (auto &wildcard_case : wildcard_cases) {
    value_ = case_value;
    wildcard_case.first->accept(*this);
    wildcard_case.second->accept(*this);
  }
}

void CodeGenBuilder::ident_pattern(PatternNode *pattern,
                                   StatementSequenceNode *statements,
                                   llvm::Value *case_value) {
  value_ = case_value;
  pattern->accept(*this);
  statements->accept(*this);
}

void CodeGenBuilder::visit(CaseStatementNode &case_stmt) {
  auto currentFunc = builder_->GetInsertBlock()->getParent();

  case_stmt.value->accept(*this);
  const auto right_value = value_;
  auto right_type = case_stmt.value->type;

  auto tail_block =
      llvm::BasicBlock::Create(builder_->getContext(), "ifTail", currentFunc);
  return_points_.push(tail_block);

  if (right_type->getNodeType() == NodeType::std_type) {
    for (auto ci : case_stmt.reachable_cases) {
      auto curr_pattern = case_stmt.get_cases()->at(ci).first.get();
      auto curr_body = case_stmt.get_cases()->at(ci).second.get();

      if (curr_pattern->getNodeType() == NodeType::literal_pattern) {
        literal_pattern(curr_pattern, curr_body, right_value);
      } else if (curr_pattern->getNodeType() == NodeType::ident_pattern) {
        ident_pattern(curr_pattern, curr_body, right_value);
      } else {
        logger_.error(curr_pattern->pos(), "UNEXPECTED PATTERN");
        exit(EXIT_FAILURE);
      }
    }
  } else if (right_type->getNodeType() == NodeType::sum_type) {
    vector<std::pair<PatternNode *, StatementSequenceNode *>> cases;
    for (auto ci : case_stmt.reachable_cases) {
      cases.emplace_back(case_stmt.get_cases()->at(ci).first.get(),
                         case_stmt.get_cases()->at(ci).second.get());
    }

    variants(cases, right_value);
  }
  builder_->CreateBr(tail_block);
  builder_->SetInsertPoint(tail_block);

  return_points_.pop();
}

llvm::Type *CodeGenBuilder::getLLVMType(TypeNode *const type) {
  // NOTE TypeDeclarations are simply new key:value pairs in types_
  // differentiation of semantically different types is gone now;
  // the parser / sema checker did that
  llvm::Type *llvm_type;
  try {
    llvm_type = types_.at(type);
  } catch (std::out_of_range &e) {
    logger_.debug("No llvm type found for " + to_string(type));
    exit(EXIT_FAILURE);
  }

  return llvm_type;
}

TypeNode *CodeGenBuilder::get_elem_ptr(
    const DeclarationNode *ref, llvm::Value *base_ptr,
    const vector<unique_ptr<SelectorNode>> &selectors) {

  TypeNode *curr_type = ref->type;

  std::vector<llvm::Value *> idxs;

  // Add zero as first index to dereference through the struct pointer.
  idxs.push_back(builder_->getInt32(0));

  for (auto &selector : selectors) {
    const auto sel = selector.get();
    if (sel->getNodeType() == NodeType::array_selector) {
      auto array_index = dynamic_cast<ArrayIndexNode *>(sel);
      auto array_type = dynamic_cast<ArrayTypeNode *>(curr_type);

      // visit index expression
      array_index->expression->accept(*this);

      // value_ is now the llvm::Value of the index expression
      idxs.push_back(value_);

      curr_type = array_type->type;
    } else if (sel->getNodeType() == NodeType::record_selector) {
      auto record_selector = dynamic_cast<RecordFieldNode *>(sel);
      auto record_type = dynamic_cast<RecordTypeNode *>(curr_type);

      // find the field index referred to by the selector ident
      auto field_index = record_type->find_field_index(*record_selector->ident);

      idxs.push_back(builder_->getInt32(field_index));
      curr_type = record_type->field_lists.at(field_index)->type;
    }
  }

  value_ = builder_->CreateInBoundsGEP(getLLVMType(ref->type), base_ptr, idxs);

  return curr_type;
}

void CodeGenBuilder::init_values(llvm::Value *ptr, llvm::Type *llvm_type) {
  auto size = module_.getDataLayout().getTypeAllocSize(llvm_type);
  builder_->CreateMemSet(ptr, builder_->getInt8(0), size, {});
}
