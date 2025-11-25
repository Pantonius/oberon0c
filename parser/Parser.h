/*
 * Parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include "ast/ast.h"

#include "scanner/Scanner.h"
#include <set>
#include <string>

#define ADVANCE_ON_TRUE true
#define ADVANCE_ON_FALSE true
#define NO_ADVANCE_ON_TRUE false
#define NO_ADVANCE_ON_FALSE false

using std::string;

class Parser {

private:
  Scanner &scanner_;
  Logger &logger_;
  std::unique_ptr<const Token> curr_token_;
  std::unique_ptr<const Token> next_token_;

  // main stuff
  const string ident();
  const string type();
  std::unique_ptr<ModuleNode> module();
  std::unique_ptr<ProcedureCallNode> procedure();
  std::unique_ptr<ImportListNode> import_list();
  std::unique_ptr<ImportNode> import();
  std::unique_ptr<ExpressionNode> expression();
  std::unique_ptr<SimpleExprNode> simple_expr();
  std::unique_ptr<StatementSequenceNode> statement_sequence();
  std::unique_ptr<StatementNode> statement();
  std::unique_ptr<IfStatementNode> if_statement();
  std::unique_ptr<CaseStatementNode> case_statement();
  std::unique_ptr<WithStatementNode> with_statement();
  std::unique_ptr<WhileStatementNode> while_statement();
  std::unique_ptr<RepeatStatementNode> repeat_statement();
  std::unique_ptr<LoopStatementNode> loop_statement();
  std::unique_ptr<ExitStatementNode> exit_statement();
  std::unique_ptr<ReturnStatementNode> return_statement();
  std::unique_ptr<ClauseNode> clause();
  std::unique_ptr<CaseLabelListNode> case_label_list();
  std::unique_ptr<CaseLabelsNode> case_labels();
  std::unique_ptr<TermNode> term();
  RelationType relation();
  SignType sign();
  AddOperatorType add_operator();
  MulOperatorType mul_operator();

  std::unique_ptr<DeclarationSequenceNode> declaration_sequence();
  std::unique_ptr<ConstDeclarationNode> const_declaration();
  std::unique_ptr<TypeDeclarationNode> type_declaration();
  std::vector<std::unique_ptr<VarDeclarationNode>> var_declarations();
  std::unique_ptr<ProcedureDeclarationNode> procedure_declaration();
  std::unique_ptr<ProcedureHeadingNode> procedure_heading();
  std::unique_ptr<ProcedureBodyNode> procedure_body();
  std::unique_ptr<FunctionCallNode> function_call();
  std::unique_ptr<DesignatorNode> designator();
  std::unique_ptr<ActualParametersNode> actual_parameters();
  std::unique_ptr<SetNode> set();
  std::unique_ptr<ElementNode> element();
  std::unique_ptr<FactorNode> factor();
  std::unique_ptr<AssignmentNode> assignment();
  std::unique_ptr<QualIdentNode> qual_ident();
  std::unique_ptr<FormalParametersNode> formal_parameters();
  std::unique_ptr<FPSectionNode> fp_section();
  std::unique_ptr<FormalTypeNode> formal_type();

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
  std::unique_ptr<const ModuleNode> parse();
};

#endif // OBERON0C_PARSER_H
