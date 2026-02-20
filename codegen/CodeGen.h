#ifndef OBERON0C_CODEGEN_H
#define OBERON0C_CODEGEN_H

#include "parser/ast/ASTContext.h"
#include "parser/ast/NodeVisitor.h"
#include "parser/ast/TypeNode.h"
#include "util/Logger.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
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
#include <memory>

using std::map;

enum class OutputFileType { AssemblyFile, LLVMIRFile, ObjectFile };

class CodeGenBuilder final : private NodeVisitor {
  friend class CodeGenBuilderTest;

public:
  CodeGenBuilder(Logger &logger, llvm::Module &mod)
      : logger_(logger),
        builder_(std::make_unique<llvm::IRBuilder<>>(mod.getContext())),
        module_(mod) {};
  ~CodeGenBuilder() override = default;

  void build(ASTContext &);

private:
  Logger &logger_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Module &module_;
  map<const TypeNode *, llvm::Type *> types_;
  map<const DeclarationNode *, llvm::Value *> values_;
  llvm::Value *value_;
  map<string, llvm::Function *> functions_;

  void visit(ArrayTypeNode &) override final;
  void visit(AssignmentNode &) override final;
  void visit(IfStatementNode &) override final;
  void visit(ElsIfStatementNode &) override final;
  void visit(ModuleNode &) override final;
  void visit(ConstDeclarationNode &) override final;
  void visit(VarDeclarationNode &) override final;
  void visit(TypeDeclarationNode &) override final;
  void visit(ParamDeclarationNode &) override final;
  void visit(ProcedureCallNode &) override final;
  void visit(ProcedureDeclarationNode &) override final;
  void visit(ProcedureTypeNode &) override final;
  void visit(IdentExpressionNode &) override final;
  void visit(BinaryExpressionNode &) override final;
  void visit(UnaryExpressionNode &) override final;
  void visit(NumberExpressionNode &) override final;
  void visit(BooleanExpressionNode &) override final;
  void visit(RecordTypeNode &) override final;
  void visit(RepeatStatementNode &) override final;
  void visit(SelectorNode &) override final;
  void visit(StatementSequenceNode &) override final;
  void visit(IdentNode &) override final;
  void visit(IdentTypeNode &) override final;
  void visit(StdTypeNode &) override final;
  void visit(FieldNode &) override final;
  void visit(WhileStatementNode &) override final;

  llvm::Type *getLLVMType(TypeNode *);
  TypeNode *
  traverse_selectors(const DeclarationNode *ref,
                     const vector<unique_ptr<SelectorNode>>::iterator start,
                     const vector<unique_ptr<SelectorNode>>::iterator end);
};

class CodeGen final {
private:
  Logger &logger_;

  std::unique_ptr<llvm::TargetMachine> init();
  void emit(std::unique_ptr<llvm::TargetMachine>, std::unique_ptr<llvm::Module>,
            const string &, const OutputFileType);
  void test_unique_ptr(std::unique_ptr<llvm::TargetMachine> tm);

public:
  CodeGen(Logger &logger) : logger_(logger) {};
  ~CodeGen() = default;

  void build(ASTContext &, string);
};

#endif // OBERON0C_CODEGEN_H
