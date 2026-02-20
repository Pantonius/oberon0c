#include "global.h"
#include "parser/Parser.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <memory>
#include <utility>

using namespace std;

TEST_CASE("Sema Ident Expression", "[sema][expression][ident]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Undeclared ident") {
    auto expr = sema.onIdentExpression(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "undeclared"), {});

    REQUIRE(expr->type == nullptr);
  }

  SECTION("No selectors") {
    auto expr = sema.onIdentExpression(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "no_selectors"), {});

    REQUIRE(expr->getNodeType() == NodeType::ident_expression);
    auto ident_expr = dynamic_cast<const IdentExpressionNode *>(expr.get());
    REQUIRE(ident_expr->selectors.size() == 0);
  }
}

TEST_CASE("Sema Binary Expression", "[sema][expression][binary]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Foldable add expression") {
    auto num_a = 20;
    auto num_b = 33;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::plus,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == num_a + num_b);
  }

  SECTION("Foldable mult expression") {
    auto num_a = 20;
    auto num_b = 33;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::times,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == num_a * num_b);
  }

  SECTION("Foldable minus expression") {
    auto num_a = 20;
    auto num_b = 33;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::minus,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == num_a - num_b);
  }

  SECTION("Foldable div expression (P, P)") {
    auto num_a = 8;
    auto num_b = 3;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::div,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == 2);
  }

  SECTION("Foldable div expression (P, N)") {
    auto num_a = 8;
    auto num_b = -3;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::div,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == -2);
  }

  SECTION("Foldable div expression (N, P)") {
    auto num_a = -8;
    auto num_b = 3;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::div,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == -2);
  }

  SECTION("Foldable div expression (N, N)") {
    auto num_a = -8;
    auto num_b = -3;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::div,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == 2);
  }

  SECTION("Foldable modulo expression (P, P)") {
    const auto num_a = 3;
    const auto num_b = 4;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::mod,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    REQUIRE(expr_number->value == 3);
  }

  SECTION("Foldable modulo expression (P, N)") {
    const auto num_a = 3;
    const auto num_b = -4;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::mod,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    // NOTE euclidean modulo
    REQUIRE(expr_number->value == 3);
  }

  SECTION("Foldable modulo expression (N, P)") {
    const auto num_a = -3;
    const auto num_b = 4;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::mod,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    // NOTE euclidean modulo
    REQUIRE(expr_number->value == 1);
  }

  SECTION("Foldable modulo expression (N, N)") {
    const auto num_a = -3;
    const auto num_b = -4;
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_a,
                                               ASTContext::INTEGER.get()),
        BinaryOpType::mod,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, num_b,
                                               ASTContext::INTEGER.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::number);
    auto expr_number =
        dynamic_cast<const NumberExpressionNode *>(binary_expr.get());
    // NOTE euclidean modulo
    REQUIRE(expr_number->value == 1);
  }

  SECTION("Foldable and expression (T,T)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_and,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == true);
  }

  SECTION("Foldable and expression (T,F)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_and,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == false);
  }

  SECTION("Foldable and expression (F,T)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_and,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == false);
  }

  SECTION("Foldable and expression (F,F)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_and,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == false);
  }

  SECTION("Foldable or expression (T,T)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_or,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == true);
  }

  SECTION("Foldable and expression (T,F)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_or,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == true);
  }

  SECTION("Foldable and expression (F,T)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_or,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, true,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == true);
  }

  SECTION("Foldable and expression (F,F)") {
    auto binary_expr = sema.onBinaryExpression(
        EMPTY_POS,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()),
        BinaryOpType::b_or,
        std::make_unique<BooleanExpressionNode>(EMPTY_POS, false,
                                                ASTContext::BOOLEAN.get()));
    REQUIRE(binary_expr->getNodeType() == NodeType::boolean);
    auto expr_bool =
        dynamic_cast<const BooleanExpressionNode *>(binary_expr.get());
    REQUIRE(expr_bool->value == false);
  }
}
