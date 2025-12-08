/*
 * Parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include "global.h"

#include "ast/ModuleNode.h"

#include "scanner/Scanner.h"
#include "scanner/Token.h"
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#define ADVANCE_ON_TRUE true
#define ADVANCE_ON_FALSE true
#define NO_ADVANCE_ON_TRUE false
#define NO_ADVANCE_ON_FALSE false

using std::optional;
using std::string;
using std::unique_ptr;

class Parser {

private:
  Scanner &scanner_;
  Logger &logger_;
  unique_ptr<const Token> last_token_;

  // TODO condense everything down
  RelationType relation();
  SignType sign();
  int number();
  string ident();
  unique_ptr<ArrayTypeNode> array_type();
  unique_ptr<ConstDeclarationNode> const_declaration();
  unique_ptr<DeclarationSequenceNode> declaration_sequence();
  unique_ptr<ExpressionNode> expression();
  unique_ptr<IfStatementNode> if_statement();
  unique_ptr<ModuleNode> module();
  unique_ptr<ProcedureCallNode> procedure();
  unique_ptr<RecordTypeNode> record_type();
  unique_ptr<RepeatStatementNode> repeat_statement();
  unique_ptr<SelectorNode> selector();
  unique_ptr<SimpleExprNode> simple_expr();
  unique_ptr<StatementNode> statement();
  unique_ptr<StatementSequenceNode> statement_sequence();
  unique_ptr<TermNode> term();
  unique_ptr<TypeDeclarationNode> type_declaration();
  unique_ptr<TypeNode> type();
  unique_ptr<WhileStatementNode> while_statement();
  std::vector<string> ident_list();
  void var_declarations(std::vector<unique_ptr<VarDeclarationNode>> &vars);
  AddOperatorType add_operator();
  MulOperatorType mul_operator();
  unique_ptr<ActualParametersNode> actual_parameters();
  unique_ptr<AssignmentNode> assignment();
  unique_ptr<AssignmentNode> assignment(string ident);
  unique_ptr<FPSectionNode> fp_section();
  unique_ptr<FactorNode> factor();
  unique_ptr<FormalParametersNode> formal_parameters();
  unique_ptr<ProcedureBodyNode> procedure_body();
  unique_ptr<ProcedureCallNode> procedure_call();
  unique_ptr<ProcedureCallNode> procedure_call(string ident);
  unique_ptr<ProcedureDeclarationNode> procedure_declaration();
  unique_ptr<ProcedureHeadingNode> procedure_heading();

  // helpers
  bool peek_check_token_type(TokenType tokenType, bool advanceOnTrue = false);
  optional<TokenType>
  peek_check_token_type_within(std::set<TokenType> expectedTypes,
                               bool advanceOnTrue = true);
  bool expect_token_type(TokenType expectedType, bool advanceOnTrue = true,
                         bool advanceOnFalse = false);
  bool expect_token_type_within(std::set<TokenType> expectedTypes,
                                bool advanceOnTrue = true,
                                bool advanceOnFalse = false);

  bool peek_array_type();
  bool peek_assignment();
  bool peek_assignment_without_ident();
  bool peek_char_constant();
  bool peek_expression();
  bool peek_factor();
  bool peek_formal_parameters();
  bool peek_ident();
  bool peek_number(bool advanceOnTrue = false);
  bool peek_procedure_call_without_ident();
  bool peek_record_type();
  bool peek_sign();
  bool peek_simple_expr();
  bool peek_string();
  bool peek_term();

public:
  explicit Parser(Scanner &scanner, Logger &logger)
      : scanner_(scanner), logger_(logger) {};
  ~Parser() = default;
  unique_ptr<ModuleNode> parse();
};

#endif // OBERON0C_PARSER_H
