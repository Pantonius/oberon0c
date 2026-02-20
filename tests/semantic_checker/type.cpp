#include "global.h"
#include "parser/Parser.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <utility>

using namespace std;

TEST_CASE("Sema Array Type", "[sema][type][array]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Constant expression") {
    auto dim = 49;
    auto expr = std::make_unique<NumberExpressionNode>(
        EMPTY_POS, dim, ASTContext::INTEGER.get());

    auto array_type =
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER.get());

    REQUIRE(array_type->expression->value == dim);
  }

  SECTION("Constant negative expression") {
    auto dim = -49;
    auto expr = std::make_unique<NumberExpressionNode>(
        EMPTY_POS, dim, ASTContext::INTEGER.get());

    REQUIRE_THROWS_AS(
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER.get()),
        NegativeIntegerException);
  }

  SECTION("Non-constant expression") {
    vector<unique_ptr<IdentNode>> var_idents;
    var_idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "nonConst"));

    auto var_decl = sema.onVars(EMPTY_POS, std::move(var_idents),
                                ASTContext::INTEGER.get());

    auto expr = sema.onIdentExpression(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "nonConst"), {});

    REQUIRE_THROWS_AS(
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER.get()),
        NonConstException);
  }
}
