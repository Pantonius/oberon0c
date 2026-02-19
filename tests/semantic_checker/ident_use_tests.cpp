#include "global.h"
#include "parser/Parser.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
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
  // i : INTEGER

  auto array_type =
      sema.onArrayType(EMPTY_POS,
                       std::make_unique<NumberExpressionNode>(
                           EMPTY_POS, 4, ASTContext::INTEGER.get()),
                       ASTContext::INTEGER.get());

  vector<unique_ptr<IdentNode>> field1;
  field1.emplace_back(std::make_unique<IdentNode>(EMPTY_POS, "field1"));

  vector<unique_ptr<IdentNode>> field2;
  field2.emplace_back(std::make_unique<IdentNode>(EMPTY_POS, "field2"));

  vector<pair<vector<unique_ptr<IdentNode>>, TypeNode *>> field_list;
  field_list.emplace_back(std::move(field1), array_type);
  field_list.emplace_back(std::move(field2), ASTContext::INTEGER.get());

  auto rec_type = sema.onRecordType(EMPTY_POS, std::move(field_list));

  vector<unique_ptr<VarDeclarationNode>> rec_vars;
  {
    vector<unique_ptr<IdentNode>> idents;

    idents.emplace_back(make_unique<IdentNode>(EMPTY_POS, "rec"));

    rec_vars = sema.onVars(EMPTY_POS, std::move(idents), rec_type);
  }

  vector<unique_ptr<VarDeclarationNode>> int_vars;
  {
    vector<unique_ptr<IdentNode>> idents;

    idents.emplace_back(make_unique<IdentNode>(EMPTY_POS, "i"));

    int_vars =
        sema.onVars(EMPTY_POS, std::move(idents), ASTContext::INTEGER.get());
  }

  SECTION("Is in enclosing scope") {
    // PROCEDURE a;
    // BEGIN
    //  rec.field2 := 3
    // END

    ProcedureTypeNode proc_type(EMPTY_POS);
    unique_ptr<IdentNode> proc_ident = make_unique<IdentNode>(EMPTY_POS, "a");
    auto proc = sema.onProcedureDeclaration(EMPTY_POS, std::move(proc_ident),
                                            &proc_type);

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

  SECTION("Is in array bounds") {
    // rec.field1[3]
    auto rec_ident = make_unique<IdentNode>(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(make_unique<ArrayIndexNode>(
        EMPTY_POS, make_unique<NumberExpressionNode>(
                       EMPTY_POS, 3, ASTContext::INTEGER.get())));

    auto assign = sema.onIdentExpression(EMPTY_POS, std::move(rec_ident),
                                         std::move(selectors));

    REQUIRE(logger.getErrorCount() == 0);
  }

  SECTION("Is out of array bounds") {
    // rec.field1[3]
    auto rec_ident = make_unique<IdentNode>(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(make_unique<ArrayIndexNode>(
        EMPTY_POS, make_unique<NumberExpressionNode>(
                       EMPTY_POS, 6, ASTContext::INTEGER.get())));

    auto assign = sema.onIdentExpression(EMPTY_POS, std::move(rec_ident),
                                         std::move(selectors));

    REQUIRE(logger.getErrorCount() == 1);
  }

  SECTION("variable as array index") {
    // rec.field1[i]
    auto rec_ident = make_unique<IdentNode>(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(sema.onArrayIndex(
        EMPTY_POS, sema.onIdentExpression(
                       EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "i"), {})));

    auto array_ident_expr = dynamic_cast<IdentExpressionNode *>(
        sema.onIdentExpression(EMPTY_POS, std::move(rec_ident),
                               std::move(selectors))
            .release());

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(array_ident_expr->type == ASTContext::INTEGER.get());
    REQUIRE(!array_ident_expr->is_const());

    delete array_ident_expr;
  }

  SECTION("variable and constant as array index") {
    // rec.field1[i - 2]
    auto rec_ident = make_unique<IdentNode>(EMPTY_POS, "rec");

    std::vector<std::unique_ptr<SelectorNode>> selectors;
    selectors.emplace_back(std::make_unique<RecordFieldNode>(
        EMPTY_POS, make_unique<IdentNode>(EMPTY_POS, "field1")));
    selectors.emplace_back(sema.onArrayIndex(
        EMPTY_POS,
        sema.onBinaryExpression(
            EMPTY_POS,
            sema.onIdentExpression(EMPTY_POS,
                                   make_unique<IdentNode>(EMPTY_POS, "i"), {}),
            BinaryOpType::minus,
            make_unique<NumberExpressionNode>(EMPTY_POS, 2,
                                              ASTContext::INTEGER.get()))));

    auto array_ident_expr = dynamic_cast<IdentExpressionNode *>(
        sema.onIdentExpression(EMPTY_POS, std::move(rec_ident),
                               std::move(selectors))
            .release());

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(array_ident_expr->type == ASTContext::INTEGER.get());
    REQUIRE(!array_ident_expr->is_const());

    delete array_ident_expr;
  }
}
