#include "global.h"
#include "parser/Parser.h"
#include "parser/SymbolTable.h"
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
  IdentTypeNode int_type(
      EMPTY_POS, unique_ptr<IdentNode>(new IdentNode(EMPTY_POS, "INTEGER")));

  ArrayTypeNode array_type(
      EMPTY_POS, std::make_unique<NumberExpressionNode>(EMPTY_POS, 4, nullptr),
      &int_type);

  vector<unique_ptr<FieldNode>> rec_fields;
  rec_fields.emplace_back(make_unique<FieldNode>(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1"), &array_type));
  symbol_table.insert(*rec_fields[0]->ident.get(), rec_fields[0]->type);

  rec_fields.emplace_back(make_unique<FieldNode>(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field2"), &int_type));
  symbol_table.insert(*rec_fields[1]->ident.get(), rec_fields[1]->type);

  RecordTypeNode rec_type(EMPTY_POS, rec_fields);

  VarDeclarationNode rec_var(
      EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "rec"), &rec_type);

  symbol_table.insert(*rec_var.ident.get(), rec_var.type);

  SECTION("Test 'rec' lookup") {
    IdentNode rec_ident(EMPTY_POS, "rec");
    auto rec_lookup = symbol_table.lookup_type(rec_ident);

    REQUIRE(rec_lookup == rec_var.type);
  }

  SECTION("Test 'rec.field1' lookup") {
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1")));

    auto field1_lookup = symbol_table.lookup_type(rec_ident, selectors);

    // Should be the type of the record field
    REQUIRE(field1_lookup == rec_fields[0]->type);
  }

  SECTION("Test 'rec.field1[2]' lookup") {
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(std::make_unique<ArrayIndexNode>(
        EMPTY_POS,
        std::make_unique<NumberExpressionNode>(EMPTY_POS, 2, nullptr)));

    auto field1_elem_lookup = symbol_table.lookup_type(rec_ident, selectors);

    // Should be the type of the array
    REQUIRE(field1_elem_lookup == array_type.type);
  }

  SECTION("Test 'rec.field2' lookup") {
    IdentNode rec_ident(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field2")));

    auto field2_lookup = symbol_table.lookup_type(rec_ident, selectors);

    // Should be the type of the record field
    REQUIRE(field2_lookup == rec_fields[1]->type);
  }
}
