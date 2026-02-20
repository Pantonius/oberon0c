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

TEST_CASE("Sema Const Declaration", "[sema][const_decl]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Simple literal expression") {
    auto number = 42;
    auto const_decl = sema.onConst(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "simple_literal"),
        std::make_unique<NumberExpressionNode>(EMPTY_POS, number,
                                               ASTContext::INTEGER.get()));

    REQUIRE(const_decl->expression->getNodeType() == NodeType::number);
    auto const_decl_number = dynamic_cast<const NumberExpressionNode *>(
        const_decl->expression.get());
    REQUIRE(const_decl_number->value == number);
  }

  SECTION("Foldable expression") {
    auto num_a = 20;
    auto num_b = 33;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::plus,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    auto const_decl = sema.onConst(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "foldable"),
        std::move(binary_expr));

    REQUIRE(const_decl->expression->getNodeType() == NodeType::number);
    auto const_decl_number = dynamic_cast<const NumberExpressionNode *>(
        const_decl->expression.get());
    REQUIRE(const_decl_number->value == num_a + num_b);
  }

  SECTION("Non-constant expression") {
    vector<unique_ptr<IdentNode>> var_idents;
    var_idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "non"));

    auto var_decl = sema.onVars(EMPTY_POS, std::move(var_idents),
                                ASTContext::INTEGER.get());
    auto ident_expr = sema.onIdentExpression(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "non"), {});

    REQUIRE_THROWS_AS(
        sema.onConst(EMPTY_POS,
                     std::make_unique<IdentNode>(EMPTY_POS, "non_const"),
                     std::move(ident_expr)),
        NonConstException);
  }

  SECTION("No expression") {
    REQUIRE_THROWS_AS(
        sema.onConst(EMPTY_POS,
                     std::make_unique<IdentNode>(EMPTY_POS, "no_expr"),
                     nullptr),
        UndeclaredArgumentException);
  }
}
