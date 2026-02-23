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
    auto expr = std::make_unique<NumberExpressionNode>(EMPTY_POS, dim);

    auto array_type =
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER);

    REQUIRE(array_type->expression->value == dim);
  }

  SECTION("Constant negative expression") {
    auto dim = -49;
    auto expr = std::make_unique<NumberExpressionNode>(EMPTY_POS, dim);

    REQUIRE_THROWS_AS(
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER),
        NegativeIntegerException);
  }

  SECTION("Non-constant expression") {
    vector<unique_ptr<IdentNode>> var_idents;
    var_idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "nonConst"));

    auto var_decl =
        sema.onVars(EMPTY_POS, std::move(var_idents), ASTContext::INTEGER);

    auto expr = sema.onIdentExpression(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "nonConst"), {});

    REQUIRE_THROWS_AS(
        sema.onArrayType(EMPTY_POS, std::move(expr), ASTContext::INTEGER),
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

    fields.emplace_back(std::move(var_idents_int), ASTContext::INTEGER);

    vector<unique_ptr<IdentNode>> var_idents_bool;
    var_idents_bool.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field3"));
    var_idents_bool.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field4"));

    fields.emplace_back(std::move(var_idents_bool), ASTContext::BOOLEAN);

    auto record = sema.onRecordType(EMPTY_POS, std::move(fields));

    REQUIRE(record->field_lists[0]->ident->value == "field1");
    REQUIRE(record->field_lists[0]->type == ASTContext::INTEGER);
    REQUIRE(record->field_lists[1]->ident->value == "field2");
    REQUIRE(record->field_lists[1]->type == ASTContext::INTEGER);

    REQUIRE(record->field_lists[2]->ident->value == "field3");
    REQUIRE(record->field_lists[2]->type == ASTContext::BOOLEAN);
    REQUIRE(record->field_lists[3]->ident->value == "field4");
    REQUIRE(record->field_lists[3]->type == ASTContext::BOOLEAN);
  }

  SECTION("Record field uniqueness") {
    vector<pair<vector<unique_ptr<IdentNode>>, TypeNode *>> fields;

    vector<unique_ptr<IdentNode>> var_idents_int;
    var_idents_int.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field1"));

    fields.emplace_back(std::move(var_idents_int), ASTContext::INTEGER);

    vector<unique_ptr<IdentNode>> var_idents_bool;
    var_idents_bool.push_back(std::make_unique<IdentNode>(EMPTY_POS, "field1"));

    fields.emplace_back(std::move(var_idents_bool), ASTContext::BOOLEAN);

    REQUIRE_THROWS_AS(sema.onRecordType(EMPTY_POS, std::move(fields)),
                      DuplicateFieldException);
  }
}

TEST_CASE("Procedure Type", "[sema][type][procedure]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "Module");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  SECTION("Procedure parameter uniqueness") {
    vector<std::tuple<vector<unique_ptr<IdentNode>>, bool, TypeNode *>>
        formal_parameters;

    vector<unique_ptr<IdentNode>> idents;
    idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "param1"));
    idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "param1"));

    formal_parameters.emplace_back(std::move(idents), false,
                                   ASTContext::INTEGER);

    REQUIRE_THROWS_AS(
        sema.onProcedureType(EMPTY_POS, std::move(formal_parameters)),
        DuplicateFieldException);
  }

  SECTION("Procedure empty parameter type") {
    vector<std::tuple<vector<unique_ptr<IdentNode>>, bool, TypeNode *>>
        formal_parameters;

    vector<unique_ptr<IdentNode>> idents;
    idents.push_back(std::make_unique<IdentNode>(EMPTY_POS, "param1"));

    formal_parameters.emplace_back(std::move(idents), false, nullptr);

    auto procedure =
        sema.onProcedureType(EMPTY_POS, std::move(formal_parameters));

    // should not throw any exceptions; just continue as declared
    // TODO: reconsider
    REQUIRE(procedure->formal_parameters.at(0)->ident->value == "param1");
    REQUIRE(procedure->formal_parameters.at(0)->type == nullptr);
  }
}
