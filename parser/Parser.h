/*
 * Parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include "ast/ActualParametersNode.h"
#include "ast/AssignmentNode.h"
#include "ast/CaseStatementNode.h"
#include "ast/ConstDeclarationNode.h"
#include "ast/DeclarationSequenceNode.h"
#include "ast/DesignatorNode.h"
#include "ast/ElementNode.h"
#include "ast/ExitStatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/FPSectionNode.h"
#include "ast/FormalParametersNode.h"
#include "ast/FormalTypeNode.h"
#include "ast/FunctionCallNode.h"
#include "ast/IfStatementNode.h"
#include "ast/ImportNode.h"
#include "ast/LoopStatementNode.h"
#include "ast/ModuleNode.h"
#include "ast/ProcedureCallNode.h"
#include "ast/ProcedureDeclarationNode.h"
#include "ast/QualIdentNode.h"
#include "ast/RepeatStatementNode.h"
#include "ast/ReturnStatementNode.h"
#include "ast/SetNode.h"
#include "ast/SimpleExprNode.h"
#include "ast/StatementNode.h"
#include "ast/StatementSequenceNode.h"
#include "ast/TermNode.h"
#include "ast/TypeDeclarationNode.h"
#include "ast/VarDeclarationNode.h"
#include "ast/WhileStatementNode.h"
#include "ast/WithStatementNode.h"

#include "scanner/Scanner.h"
#include <memory>
#include <set>
#include <string>

#define ADVANCE_ON_TRUE true
#define ADVANCE_ON_FALSE true
#define NO_ADVANCE_ON_TRUE false
#define NO_ADVANCE_ON_FALSE false

using std::string;
using std::unique_ptr;

class Parser {

private:
  Scanner &scanner_;
  Logger &logger_;
  unique_ptr<const Token> curr_token_;
  unique_ptr<const Token> next_token_;

  // TODO condense everything down
  const string ident();
  const string type();
  unique_ptr<ModuleNode> module();
  unique_ptr<ProcedureCallNode> procedure();
  unique_ptr<ImportListNode> import_list();
  unique_ptr<ImportNode> import();
  unique_ptr<ExpressionNode> expression();
  unique_ptr<SimpleExprNode> simple_expr();
  unique_ptr<StatementSequenceNode> statement_sequence();
  unique_ptr<StatementNode> statement();
  unique_ptr<IfStatementNode> if_statement();
  unique_ptr<CaseStatementNode> case_statement();
  unique_ptr<WithStatementNode> with_statement();
  unique_ptr<WhileStatementNode> while_statement();
  unique_ptr<RepeatStatementNode> repeat_statement();
  unique_ptr<LoopStatementNode> loop_statement();
  unique_ptr<ExitStatementNode> exit_statement();
  unique_ptr<ReturnStatementNode> return_statement();
  unique_ptr<ClauseNode> clause();
  unique_ptr<CaseLabelListNode> case_label_list();
  unique_ptr<CaseLabelsNode> case_labels();
  unique_ptr<TermNode> term();
  unique_ptr<DeclarationSequenceNode> declaration_sequence();
  unique_ptr<ConstDeclarationNode> const_declaration();
  unique_ptr<TypeDeclarationNode> type_declaration();
  std::vector<unique_ptr<VarDeclarationNode>> var_declarations();
  unique_ptr<ProcedureDeclarationNode> procedure_declaration();
  unique_ptr<ProcedureHeadingNode> procedure_heading();
  unique_ptr<ProcedureBodyNode> procedure_body();
  unique_ptr<FunctionCallNode> function_call();
  unique_ptr<DesignatorNode> designator();
  unique_ptr<ActualParametersNode> actual_parameters();
  unique_ptr<SetNode> set();
  unique_ptr<ElementNode> element();
  unique_ptr<FactorNode> factor();
  unique_ptr<AssignmentNode> assignment();
  unique_ptr<QualIdentNode> qual_ident();
  unique_ptr<FormalParametersNode> formal_parameters();
  unique_ptr<FPSectionNode> fp_section();
  unique_ptr<FormalTypeNode> formal_type();
  RelationType relation();
  SignType sign();
  AddOperatorType add_operator();
  MulOperatorType mul_operator();

  // helpers
  bool peek_check_token_type(TokenType tokenType, bool advanceOnTrue = false);
  bool peek_check_token_type_within(std::set<TokenType> expectedTypes,
                                    bool advanceOnTrue = true);
  bool expect_token_type(TokenType expectedType, bool advanceOnTrue = true,
                         bool advanceOnFalse = false);
  bool expect_token_type_within(std::set<TokenType> expectedTypes,
                                bool advanceOnTrue = true,
                                bool advanceOnFalse = false);

  bool peek_formal_parameters();
  bool peek_assignment();
  bool peek_designator();
  bool peek_set();
  bool peek_expression();

public:
  explicit Parser(Scanner &scanner, Logger &logger)
      : scanner_(scanner), logger_(logger) {};
  ~Parser() = default;
  unique_ptr<ModuleNode> parse();
};

#endif // OBERON0C_PARSER_H
