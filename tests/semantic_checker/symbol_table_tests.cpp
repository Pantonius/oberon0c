#include "global.h"
#include "parser/Parser.h"
#include "parser/SymbolTable.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

using std::make_unique;
using std::unique_ptr;

TEST_CASE("Test SymbolTable", "[symbol_table]") {
  Logger logger;
  SymbolTable symbol_table(logger);

  symbol_table.beginScope();

  // VAR rec : RECORD
  //   field1 : ARRAY 4 OF INTEGER;
  //   field2 : INTEGER
  // END;
  ArrayTypeNode array_type(EMPTY_POS,
                           make_unique<NumberExpressionNode>(
                               EMPTY_POS, 4, ASTContext::INTEGER.get()),
                           ASTContext::INTEGER.get());

  symbol_table.beginScope();

  vector<unique_ptr<VarDeclarationNode>> rec_fields;
  rec_fields.emplace_back(make_unique<VarDeclarationNode>(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1"), &array_type));
  symbol_table.insert(*rec_fields[0]->ident.get(), rec_fields[0].get());

  rec_fields.emplace_back(make_unique<VarDeclarationNode>(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field2"),
      ASTContext::INTEGER.get()));
  symbol_table.insert(*rec_fields[1]->ident.get(), rec_fields[1].get());

  RecordTypeNode rec_type(EMPTY_POS, std::move(rec_fields));

  symbol_table.endScope();

  VarDeclarationNode rec_var(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "rec"), &rec_type);

  symbol_table.insert(*rec_var.ident.get(), &rec_var);

  SECTION("Test 'rec' lookup") {
    // rec
    IdentNode rec_ident(EMPTY_POS, "rec");
    auto rec_lookup = symbol_table.lookup(rec_ident);

    REQUIRE(rec_lookup == &rec_var);
  }

  SECTION("Test 'rec.field1' lookup") {
    // rec.field1
    auto rec_ident = std::make_unique<IdentNode>(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1")));

    auto type = symbol_table.lookup_type(*rec_ident, selectors);
    IdentExpressionNode ident_expr(EMPTY_POS, std::move(rec_ident),
                                   std::move(selectors), type);

    // Should be the type of the record field
    REQUIRE(ident_expr.type == rec_type.field_lists[0]->type);
  }

  SECTION("Test test array selector lookup: 'rec.field1[2]'") {
    // rec.field1[2]
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(std::make_unique<ArrayIndexNode>(
        EMPTY_POS, std::make_unique<NumberExpressionNode>(
                       EMPTY_POS, 2, ASTContext::INTEGER.get())));

    auto type = symbol_table.lookup_type(rec_ident, selectors);
    IdentExpressionNode ident_expr(EMPTY_POS,
                                   std::make_unique<IdentNode>(rec_ident),
                                   std::move(selectors), type);

    // Should be the type of the array
    REQUIRE(ident_expr.type == array_type.type);
  }

  SECTION("Test record field lookup: 'rec.field2'") {
    // rec.field2
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field2")));

    auto type = symbol_table.lookup_type(rec_ident, selectors);
    IdentExpressionNode ident_expr(EMPTY_POS,
                                   std::make_unique<IdentNode>(rec_ident),
                                   std::move(selectors), type);

    // Should be the type of the record field
    REQUIRE(ident_expr.type == rec_type.field_lists[1]->type);
  }

  SECTION("Test non existing field lookup: 'rec.field3'") {
    // rec.field3
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field3")));

    REQUIRE_THROWS_AS(symbol_table.lookup_type(rec_ident, selectors),
                      FieldNotFoundException);
  }

  SECTION("Test wrong selector type: 'rec[2]'") {
    // rec[2]
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<ArrayIndexNode>(
        EMPTY_POS, std::make_unique<NumberExpressionNode>(
                       EMPTY_POS, 2, ASTContext::INTEGER.get())));

    REQUIRE_THROWS_AS(symbol_table.lookup_type(rec_ident, selectors),
                      WrongTypeException);
  }

  SECTION("Test array out of bounds") {
    // rec.field1[5]
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(std::make_unique<ArrayIndexNode>(
        EMPTY_POS, std::make_unique<NumberExpressionNode>(
                       EMPTY_POS, 5, ASTContext::INTEGER.get())));

    REQUIRE_THROWS_AS(symbol_table.lookup_type(rec_ident, selectors),
                      OutOfRangeException);
  }
}
