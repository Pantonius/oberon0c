#include "codegen/CodeGen.h"
#include "global.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <cstdint>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <memory>

static auto ctx = std::make_unique<llvm::LLVMContext>();
static auto module = std::make_unique<llvm::Module>("test", *ctx);

class CodeGenBuilderTest {
protected:
  Logger logger;
  std::unique_ptr<ASTContext> ast;
  CodeGenBuilder builder;

public:
  CodeGenBuilderTest() : logger(), ast(), builder(logger, *module) {}

protected:
  llvm::Value *getValue() { return builder.value_; }
  llvm::IRBuilder<> *getBuilder() { return builder.builder_.get(); }

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
    VarDeclarationNode var_decl(EMPTY_POS,
                                std::make_unique<IdentNode>(EMPTY_POS, "test"),
                                ASTContext::INTEGER.get());
    auto ident = std::make_unique<IdentNode>(EMPTY_POS, "test");
    IdentExpressionNode ident_expr(EMPTY_POS, std::move(ident), {}, &var_decl,
                                   ASTContext::INTEGER.get());

    ident_expr.accept(builder);
  }
};

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen BooleanExpressionsNode Test",
                 "[code_gen]") {
  testBoolExpression(true);

  auto value = static_cast<llvm::ConstantInt *>(getValue());
  REQUIRE(value->isOne());
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen NumberExpressionsNode Test",
                 "[code_gen]") {
  testNumberExpression(42);

  auto value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == 42);

  testNumberExpression(-42);

  value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->isNegative());
  REQUIRE(value->getSExtValue() == -42);
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen UnaryExpressionNode Test",
                 "[code_gen]") {
  auto num = GENERATE(take(10000, random(INT32_MIN, INT32_MAX)));
  testUnaryNumberExpression(num, UnaryOpType::plus);

  auto value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == num);

  testUnaryNumberExpression(num, UnaryOpType::minus);

  value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == -num);

  testUnaryBoolExpression(true, UnaryOpType::u_not);

  value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->isZero());
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen IdentExpressionNode Test",
                 "[code_gen]") {
  testIdentExpression();

  auto load = static_cast<llvm::AllocaInst *>(getValue());

  REQUIRE(load->getName() == "test");
  REQUIRE(load->getAllocatedType()->isIntegerTy());
}
