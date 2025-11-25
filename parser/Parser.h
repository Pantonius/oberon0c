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
  std::unique_ptr<ModuleNode> module();
  std::unique_ptr<ProcedureCallNode> procedure();
  std::unique_ptr<ImportListNode> import_list();
  std::unique_ptr<ImportNode> import();
  std::unique_ptr<RelationNode> relation();
  std::unique_ptr<ExpressionNode> expression();

  std::unique_ptr<DeclarationSequenceNode> declaration_sequence();
  std::unique_ptr<ConstDeclarationNode> const_declaration();
  std::unique_ptr<TypeDeclarationNode> type_declaration();
  std::vector<std::unique_ptr<VarDeclarationNode>> var_declarations();
  std::unique_ptr<ProcedureDeclarationNode> procedure_declaration();

  // helpers
  bool peek_check_token_type(TokenType tokenType, bool advanceOnTrue = false);
  bool peek_check_token_type_within(std::set<TokenType> expectedTypes,
                                    bool advanceOnTrue = true);
  bool expect_token_type(TokenType expectedType, bool advanceOnTrue = true,
                         bool advanceOnFalse = false);
  bool expect_token_type_within(std::set<TokenType> expectedTypes,
                                bool advanceOnTrue = true,
                                bool advanceOnFalse = false);

public:
  explicit Parser(Scanner &scanner, Logger &logger)
      : scanner_(scanner), logger_(logger) {};
  ~Parser() = default;
  std::unique_ptr<const ModuleNode> parse();
};

#endif // OBERON0C_PARSER_H
