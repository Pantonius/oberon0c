#ifndef OBERON0C_CODEGEN_H
#define OBERON0C_CODEGEN_H

#include "parser/ast/ASTContext.h"
#include "parser/ast/NodeVisitor.h"
#include <iostream>
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

enum class OutputFileType { AssemblyFile, LLVMIRFile, ObjectFile };

class CodeGenBuilder final : private NodeVisitor {
public:
  CodeGenBuilder(llvm::LLVMContext &ctx) : builder_(ctx) {};
  ~CodeGenBuilder() override = default;

  void build(ASTContext &);

private:
  llvm::IRBuilder<> builder_;

  void visit(ArrayTypeNode &array_type) override final;
  void visit(AssignmentNode &assignment) override final;
  void visit(ConstDeclarationNode &const_declaration) override final;
  void visit(ExpressionNode &expression) override final;
  void visit(IfStatementNode &if_statement) override final;
  void visit(ModuleNode &module_node) override final;
  void visit(ProcedureCallNode &procedure_call) override final;
  void visit(ProcedureDeclarationNode &procedure_declaration) override final;
  void visit(RecordTypeNode &record_type) override final;
  void visit(RepeatStatementNode &repeat_statement) override final;
  void visit(SelectorNode &selector) override final;
  void visit(StatementNode &statement) override final;
  void visit(StatementSequenceNode &statement_sequence) override final;
  void visit(IdentNode &ident) override final;
  void visit(FieldNode &field) override final;
  void visit(TypeNode &type) override final;
  void visit(TypeDeclarationNode &type_declaration) override final;
  void visit(ParamDeclarationNode &param_declaration) override final;
  void visit(VarDeclarationNode &var_declaration) override final;
  void visit(WhileStatementNode &while_statement) override final;
};

class CodeGen final {
public:
  CodeGen() {};
  ~CodeGen() = default;

  void build(ASTContext &, string name);
};

#endif // OBERON0C_CODEGEN_H
