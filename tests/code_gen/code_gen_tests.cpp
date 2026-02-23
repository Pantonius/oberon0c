#include "codegen/CodeGen.h"
#include "global.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <cstdint>
#include <iostream>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <stdexcept>

static auto ctx = std::make_unique<llvm::LLVMContext>();
static auto module = std::make_unique<llvm::Module>("test", *ctx);
static llvm::DataLayout data_layout = llvm::DataLayout();

std::unique_ptr<IdentNode> make_ident(std::string ident) {
  return std::make_unique<IdentNode>(EMPTY_POS, ident);
}

class CodeGenBuilderTest {
protected:
  Logger logger;
  std::unique_ptr<ASTContext> ast;
  CodeGenBuilder builder;

public:
  CodeGenBuilderTest()
      : logger(LogLevel::DEBUG, std::cout, std::cerr), ast(),
        builder(logger, *module) {
    module->setDataLayout(data_layout);

    auto main =
        module->getOrInsertFunction("main", builder.builder_->getInt32Ty());
    const auto function = cast<llvm::Function>(main.getCallee());
    const auto entry = llvm::BasicBlock::Create(builder.builder_->getContext(),
                                                "entry", function);
    builder.builder_->SetInsertPoint(entry);
  }

protected:
  const llvm::Value *getValue() { return builder.value_; }
  const llvm::IRBuilder<> *getBuilder() { return builder.builder_.get(); }
  const std::map<const TypeNode *, llvm::Type *> &getTypes() {
    return builder.types_;
  }
  const std::map<const DeclarationNode *, llvm::Value *> &getValues() {
    return builder.values_;
  }

  void testBoolExpression(bool boolean_) {
    BooleanExpressionNode boolean(EMPTY_POS, boolean_);

    boolean.accept(builder);
  }

  void testNumberExpression(int num) {
    NumberExpressionNode number(EMPTY_POS, num);

    number.accept(builder);
  }

  void testUnaryNumberExpression(int num, UnaryOpType op) {
    auto number = std::make_unique<NumberExpressionNode>(EMPTY_POS, num);
    UnaryExpressionNode u_expr(EMPTY_POS, op, std::move(number), number->type);

    u_expr.accept(builder);
  }
  void testUnaryBoolExpression(bool boolean, UnaryOpType op) {
    auto number = std::make_unique<BooleanExpressionNode>(EMPTY_POS, boolean);
    UnaryExpressionNode u_expr(EMPTY_POS, op, std::move(number), number->type);

    u_expr.accept(builder);
  }

  void testIdentExpression() {
    VarDeclarationNode var_decl(EMPTY_POS, make_ident("test"),
                                ASTContext::INTEGER);

    var_decl.accept(builder);

    auto ident = make_ident("test");
    IdentExpressionNode ident_expr(EMPTY_POS, std::move(ident), {}, &var_decl,
                                   ASTContext::INTEGER, false);

    ident_expr.accept(builder);
  }

  void testStdType(StdTypeNode *std_type) { std_type->accept(builder); }

  void testProcedureDeclaration() {
    ASTContext::INTEGER->accept(builder);
    vector<std::unique_ptr<ParamDeclarationNode>> params;
    params.emplace_back(std::make_unique<ParamDeclarationNode>(
        EMPTY_POS, make_ident("i"), false, ASTContext::INTEGER));
    ProcedureTypeNode proc_type(EMPTY_POS, std::move(params));
    ProcedureDeclarationNode proc_decl(EMPTY_POS, make_ident("TestProcedure"),
                                       &proc_type);

    proc_decl.accept(builder);
  }

  llvm::Value *testVarDeclaration() {
    VarDeclarationNode var_decl(EMPTY_POS,
                                std::make_unique<IdentNode>(EMPTY_POS, "test"),
                                ASTContext::INTEGER);

    var_decl.accept(builder);
    return builder.values_.at(&var_decl);
  }
};

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen BooleanExpressionsNode Test",
                 "[code_gen]") {
  testBoolExpression(true);

  auto value = static_cast<const llvm::ConstantInt *>(getValue());
  REQUIRE(value->isOne());
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen NumberExpressionsNode Test",
                 "[code_gen]") {
  testNumberExpression(42);

  auto value = static_cast<const llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == 42);

  testNumberExpression(-42);

  value = static_cast<const llvm::ConstantInt *>(getValue());

  REQUIRE(value->isNegative());
  REQUIRE(value->getSExtValue() == -42);
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen UnaryExpressionNode Test",
                 "[code_gen]") {
  auto num = GENERATE(take(10000, random(INT32_MIN, INT32_MAX)));
  testUnaryNumberExpression(num, UnaryOpType::plus);

  auto value = static_cast<const llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == num);

  testUnaryNumberExpression(num, UnaryOpType::minus);

  value = static_cast<const llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == -num);

  testUnaryBoolExpression(true, UnaryOpType::u_not);

  value = static_cast<const llvm::ConstantInt *>(getValue());

  REQUIRE(value->isZero());
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen IdentExpressionNode Test",
                 "[code_gen]") {
  testIdentExpression();

  auto load = llvm::cast<const llvm::LoadInst>(getValue());

  REQUIRE(load->getType()->isIntegerTy(32));
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen StdTypeNode Test", "[code_gen]") {
  testStdType(ASTContext::BOOLEAN);
  testStdType(ASTContext::INTEGER);

  try {
    auto llvm_bool_type =
        static_cast<llvm::IntegerType *>(getTypes().at(ASTContext::BOOLEAN));
    REQUIRE(llvm_bool_type->isIntegerTy(1));

    auto llvm_int_type =
        static_cast<llvm::IntegerType *>(getTypes().at(ASTContext::INTEGER));
    REQUIRE(llvm_int_type->isIntegerTy(32));
  } catch (std::out_of_range &e) {
    FAIL("llvm type not found");
  }
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen ProcedureDeclarationNode Test",
                 "[code_gen]") {
  testProcedureDeclaration();

  auto func = module->getFunction("TestProcedure");

  REQUIRE(func);
  REQUIRE(func->args().begin()->getName() == "i");
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen VarDeclarationNode Test",
                 "[code_gen]") {
  auto value = testVarDeclaration();

  REQUIRE(value);

  auto alloca = llvm::dyn_cast<llvm::AllocaInst>(value);

  REQUIRE(alloca);

  REQUIRE(alloca->getAllocatedType()->isIntegerTy(32));
  REQUIRE(alloca->getName() == "test");
}
