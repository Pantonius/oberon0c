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

TEST_CASE("Sema Record Type", "[sema][type][record]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Record fields") {
    vector<pair<vector<unique_ptr<IdentNode>>, TypeNode *>> fields;

    vector<unique_ptr<IdentNode>> var_idents_int;
    var_idents_int.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field1"));
    var_idents_int.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field2"));

    fields.emplace_back(std::move(var_idents_int), ASTContext::INTEGER.get());

    vector<unique_ptr<IdentNode>> var_idents_bool;
    var_idents_bool.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field3"));
    var_idents_bool.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field4"));

    fields.emplace_back(std::move(var_idents_bool), ASTContext::BOOLEAN.get());

    auto record = sema.onRecordType(EMPTY_POS, std::move(fields));

    REQUIRE(record->field_lists[0]->ident->value == "field1");
    REQUIRE(record->field_lists[0]->type == ASTContext::INTEGER.get());
    REQUIRE(record->field_lists[1]->ident->value == "field2");
    REQUIRE(record->field_lists[1]->type == ASTContext::INTEGER.get());

    REQUIRE(record->field_lists[2]->ident->value == "field3");
    REQUIRE(record->field_lists[2]->type == ASTContext::BOOLEAN.get());
    REQUIRE(record->field_lists[3]->ident->value == "field4");
    REQUIRE(record->field_lists[3]->type == ASTContext::BOOLEAN.get());
  }
}
