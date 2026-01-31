#include "global.h"
#include "parser/Parser.h"
#include "parser/SymbolTable.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/FPSectionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <utility>
#include <vector>

using namespace std;

TEST_CASE("Semantic Checker", "[sema]") {
  Logger logger;
  SemanticChecker sema(logger);

  auto module_ident = make_unique<IdentNode>(EMPTY_POS, "ident_use_test");

  sema.onModuleStart(EMPTY_POS, std::move(module_ident));

  // VAR array : RECORD
  //   field1 : ARRAY 4 OF INTEGER;
  //   field2 : INTEGER
  // END;
  IdentTypeNode int_type(
      EMPTY_POS, unique_ptr<IdentNode>(new IdentNode(EMPTY_POS, "INTEGER")));

  auto array_type =
      sema.onArrayType(EMPTY_POS,
                       std::make_unique<NumberExpressionNode>(
                           EMPTY_POS, 4, ASTContext::INTEGER.get()),
                       ASTContext::INTEGER.get());

  vector<unique_ptr<VarDeclarationNode>> rec_fields;
  rec_fields.emplace_back(make_unique<VarDeclarationNode>(
      EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field1"), array_type));

  rec_fields.emplace_back(make_unique<VarDeclarationNode>(
      EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field2"),
      ASTContext::INTEGER.get()));

  auto rec_type = sema.onRecordType(EMPTY_POS, std::move(rec_fields));

  vector<unique_ptr<IdentNode>> idents;

  idents.emplace_back(make_unique<IdentNode>(EMPTY_POS, "rec"));

  auto rec_vars = sema.onVars(EMPTY_POS, std::move(idents), rec_type);

  SECTION("Is in enclosing scope") {
    // PROCEDURE a;
    // BEGIN
    //  rec.field2 := 3
    // END

    ProcedureTypeNode proc_type(EMPTY_POS);
    unique_ptr<IdentNode> proc_ident = make_unique<IdentNode>(EMPTY_POS, "a");
    auto proc = sema.onProcedureStart(EMPTY_POS, std::move(proc_ident),
                                      vector<unique_ptr<FPSectionNode>>());

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "field2")));

    auto assign =
        sema.onAssign(EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "rec"),
                      std::move(selectors),
                      make_unique<NumberExpressionNode>(
                          EMPTY_POS, 3, ASTContext::INTEGER.get()));

    REQUIRE(logger.getErrorCount() == 0);
  }
}
