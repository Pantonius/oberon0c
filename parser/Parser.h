#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include "SemanticChecker.h"
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
  SemanticChecker sema_;

  BinaryOpType relation();
  int number();
  unique_ptr<IdentNode> ident();
  const ArrayTypeNode *array_type();
  unique_ptr<ConstDeclarationNode> const_declaration();
  void declaration_sequence(DeclarationSequenceNode *);
  unique_ptr<ExpressionNode> expression();
  unique_ptr<ExpressionNode> simple_expr();
  unique_ptr<ExpressionNode> factor();
  unique_ptr<ExpressionNode> term();
  unique_ptr<IfStatementNode> if_statement();
  void module();
  unique_ptr<ProcedureCallNode> procedure();
  const RecordTypeNode *record_type();
  unique_ptr<RepeatStatementNode> repeat_statement();
  vector<unique_ptr<SelectorNode>> selectors();
  unique_ptr<StatementNode> statement();
  unique_ptr<StatementSequenceNode> statement_sequence();
  unique_ptr<TypeDeclarationNode> type_declaration();
  const TypeNode *type();
  unique_ptr<WhileStatementNode> while_statement();
  std::vector<unique_ptr<IdentNode>> ident_list();
  vector<unique_ptr<VarDeclarationNode>> var_declarations();
  BinaryOpType add_operator();
  BinaryOpType mul_operator();
  unique_ptr<AssignmentNode> assignment();
  unique_ptr<AssignmentNode> assignment(unique_ptr<IdentNode> ident);
  unique_ptr<FPSectionNode> fp_section();
  unique_ptr<ProcedureCallNode> procedure_call();
  unique_ptr<ProcedureCallNode> procedure_call(unique_ptr<IdentNode> ident);
  unique_ptr<ProcedureDeclarationNode> procedure_declaration();

  UnaryOpType sign();

  // helpers
  bool peek_check_token_type(TokenType tokenType, bool advanceOnTrue = false);
  optional<TokenType>
  peek_check_token_type_within(std::set<TokenType> expectedTypes,
                               bool advanceOnTrue = false);
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
  bool peek_ident();
  bool peek_selector();
  bool peek_number(bool advanceOnTrue = false);
  bool peek_procedure_call_without_ident();
  bool peek_record_type();
  bool peek_sign();
  bool peek_simple_expr();
  bool peek_string();
  bool peek_term();
  bool peek_const_declaration();
  bool peek_type_declaration();
  bool peek_var_declaration();

public:
  explicit Parser(Scanner &scanner, Logger &logger)
      : scanner_(scanner), logger_(logger), sema_(logger) {};
  ~Parser() = default;

  ASTContext *parse();
};

#endif // OBERON0C_PARSER_H
