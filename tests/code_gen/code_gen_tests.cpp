#include "codegen/CodeGen.h"
#include "global.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/ExpressionNode.h"
#include <catch2/catch_test_macros.hpp>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
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
    BooleanExpressionNode boolean(EMPTY_POS, boolean_,
                                  ASTContext::BOOLEAN.get());

    boolean.accept(builder);
  }

  void testNumberExpression(int num) {
    NumberExpressionNode number(EMPTY_POS, num, ASTContext::INTEGER.get());

    number.accept(builder);
  }
};

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen Boolean-Expressions Test",
                 "[code_gen]") {
  testBoolExpression(true);

  auto value = static_cast<llvm::ConstantInt *>(getValue());
  REQUIRE(value->isOne());
}

TEST_CASE_METHOD(CodeGenBuilderTest, "CodeGen Number-Expressions Test",
                 "[code_gen]") {
  testNumberExpression(42);

  auto value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->getSExtValue() == 42);

  testNumberExpression(-42);

  value = static_cast<llvm::ConstantInt *>(getValue());

  REQUIRE(value->isNegative());
  REQUIRE(value->getSExtValue() == -41);
}
