#include "Parser.h"

std::unique_ptr<const ModuleNode> Parser::parse() { module(); }

/* ident = letter {letter | digit} */
const string Parser::ident() {
  if (expect_token_type(TokenType::char_literal)) {
    // Cast token pointer to IdentToken
    auto ident = dynamic_cast<const IdentToken *>(curr_token_.get());

    return ident->value();
  }

  logger_.error(curr_token_->start(), "Expected an identifier. Exiting.");
  exit(EXIT_FAILURE);
}

/* module = "MODULE" ident ";"
 *          [ ImportList ]
 *          DeclarationSequence
 *          [ "BEGIN" StatementSequence ]
 *          "END" ident "." */
std::unique_ptr<ModuleNode> Parser::module() {
  expect_token_type(TokenType::kw_module);

  auto module = std::make_unique<ModuleNode>(curr_token_->start());

  module->ident = ident();
  expect_token_type(TokenType::semicolon);

  // [
  if (expect_token_type(TokenType::kw_import, NO_ADVANCE_ON_TRUE)) {
    module->import_list = import_list();
  }
  // ]

  module->declaration_sequence = declaration_sequence();

  // [
  expect_token_type(TokenType::kw_begin, NO_ADVANCE_ON_TRUE);
  module->statement_sequence = statement_sequence();
  // ]

  expect_token_type(TokenType::kw_end);
  ident();
  expect_token_type(TokenType::period);
  expect_token_type(TokenType::eof);

  return module;
}

/* DeclarationSequence =
 *          { "CONST" { ConstDeclaration ";" }
 *          | "TYPE" { TypeDeclaration ";" }
 *          | "VAR" { VarDeclaration ";" }}
 *          | { ProcedureDeclaration ";" } */
std::unique_ptr<DeclarationSequenceNode> Parser::declaration_sequence() {
  auto declarations =
      std::make_unique<DeclarationSequenceNode>(curr_token_->start());

  // TODO expect_token_type_within maybe as a template, that returns an index if
  // within and -1 if outside
  while (expect_token_type_within(
      {TokenType::kw_const, TokenType::kw_type, TokenType::kw_var},
      NO_ADVANCE_ON_TRUE)) {
    if (peek_check_token_type(TokenType::kw_const, ADVANCE_ON_TRUE)) {
      declarations->constants.push_back(const_declaration());
    }
    if (peek_check_token_type(TokenType::kw_type, ADVANCE_ON_TRUE)) {
      declarations->types.push_back(type_declaration());
    }
    if (peek_check_token_type(TokenType::kw_var, ADVANCE_ON_TRUE)) {
      declarations->variables =
          var_declarations(); // TODO should be a merge or smth
    }
  }

  while (peek_check_token_type(TokenType::kw_procedure, NO_ADVANCE_ON_TRUE)) {
    declarations->procedures.push_back(procedure_declaration());
  }

  return declarations;
}

// ConstDeclaration = ident ["*"] "=" ConstExpression
// ConstExpression = expression
std::unique_ptr<ConstDeclarationNode> Parser::const_declaration() {
  auto const_declaration =
      std::make_unique<ConstDeclarationNode>(curr_token_->start());
  const_declaration->ident = ident();

  if (peek_check_token_type(TokenType::op_times, ADVANCE_ON_TRUE)) {
    const_declaration->exported = true;
  }

  expect_token_type(TokenType::op_eq);

  // ConstExpression
  expression();
}

// TypeDeclaration = ident "=" expression
std::unique_ptr<TypeDeclarationNode> Parser::type_declaration() {
  auto type_declaration =
      std::make_unique<TypeDeclarationNode>(curr_token_->start());

  type_declaration->ident = ident();
  expect_token_type(TokenType::op_eq);
  type_declaration->expression = expression();

  return type_declaration;
}

// VarDeclaration = IdentList ":" type
// IdentList = ident ["*"] {"," ident["*"]}
std::vector<std::unique_ptr<VarDeclarationNode>> Parser::var_declarations() {
  std::vector<std::unique_ptr<VarDeclarationNode>> vars;

  auto var_declaration =
      std::make_unique<VarDeclarationNode>(curr_token_->start());

  do {
    var_declaration->ident = ident();
    if (peek_check_token_type(TokenType::op_times, ADVANCE_ON_TRUE)) {
      var_declaration->exported = true;
    }

    vars.push_back(std::move(var_declaration));
  } while (expect_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  expect_token_type(TokenType::colon);

  type();

  return vars;
}

// expression = SimpleExpr [relation SimpleExpr]
std::unique_ptr<ExpressionNode> Parser::expression() {
  auto expression = std::make_unique<ExpressionNode>(curr_token_->start());
  expression->left_expr = simple_expr();

  if (peek_check_token_type_within(RELATION_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)) {
    expression->relation = relation();
    expression->right_expr = simple_expr();
  }

  return expression;
}

// relation = "=" | "#" | "<" | "<=" | ">" | ">=" | "IN" | "IS"
std::unique_ptr<RelationNode> Parser::relation() {
  auto relation = std::make_unique<RelationNode>(curr_token_->start());

  expect_token_type_within(RELATION_TOKEN_TYPES, ADVANCE_ON_TRUE);
  relation->type = relation_type_from_token_type(curr_token_->type());

  return relation;
}
// SimpleExpr = ["+" | "-"] term {AddOperator term}
std::unique_ptr<SimpleExprNode> Parser::simple_expr() {
  auto simple_expr = std::make_unique<SimpleExprNode>(curr_token_->start());

  if (peek_check_token_type_within(SIGN_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)) {
    simple_expr->sign = sign();
  }

  simple_expr->term = term();

  while (peek_check_token_type_within(ADD_OPERATOR_TOKEN_TYPES,
                                      NO_ADVANCE_ON_TRUE)) {
    // TODO how to put this into SimpleExprNode?
    add_operator();
    term();
  }

  return simple_expr();
}
// AddOperator = "+" | "-" | "OR"
std::unique_ptr<AddOperatorNode> Parser::add_operator() {
  auto add_operator = std::make_unique<AddOperatorNode>(curr_token_->start());

  expect_token_type_within(ADD_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  // TODO frontier
}
// term = factor {MulOperator factor}
// MulOperator = "*" | "/" | "DIV" | "MOD" | "&"
// factor = number | CharConstant | string | set | NIL | designator |

// FunctionCall | "(" expression ")" | "~" factor FunctionCall = designator "("
// [ActualParameters] ")" ActualParameters = expression {"," expression}

// designator = ident
// set = "{" [element {"," element}] "}"
// element = expression [".." expression]
// assignment = designator ":=" expression

// StatementSequence = statement {";" statement}
// statement = [ assignment | ProcedureCall | IfStatement | CaseStatement |

// LoopStatement | WithStatement | ExitStatement | ReturnStatement ]

// IfStatement = "IF" expression "THEN" StatementSequence {"ELSIF" expression
// "THEN" StatementSequence} ["ELSE" StatementSequence] "END"

// CaseStatement = "CASE" expression "OF" case {"|" case} ["ELSE"
// StatementSequence] "END" case = [CaseLabelList ":" StatementSequence]
// CaseLabelList = CaseLabels {"," CaseLabels}
// CaseLabels = ConstExpression [".." ConstExpression]

// WhileStatement = "WHILE expression "DO" StatementSequence "END"

// RepeatStatement = "REPEAT" StatementSequence "UNTIL" expression

// LoopStatement = "LOOP" StatementSequence "END"

// ExitStatement = "EXIT"

// TODO
// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ident
// ProcedureHeading = "PROCEDURE" ident ["*"] [FormalParameters]
// ProcedureBody = DeclarationSequence ["BEGIN" StatementSequence] "END"
std::vector<std::unique_ptr<ProcedureDeclarationNode>>
Parser::procedure_declarations() {}

/* ImportList = "IMPORT" import {"," import} ";" */
std::unique_ptr<ImportListNode> Parser::import_list() {
  expect_token_type(TokenType::kw_import);

  auto import_list = std::make_unique<ImportListNode>(curr_token_->start());
  import_list->list.push_back(import());

  while (expect_token_type(TokenType::comma, ADVANCE_ON_TRUE)) {
    import_list->list.push_back(import());
  }

  expect_token_type(TokenType::semicolon);
}

/* Import = ident [":=" ident] */
std::unique_ptr<ImportNode> Parser::import() {
  auto import = std::make_unique<ImportNode>(curr_token_->start());
  import->alias_ident = ident();

  if (expect_token_type(TokenType::op_becomes, ADVANCE_ON_TRUE)) {
    import->actual_ident = ident();
  }

  return import;
}

/* qualident = [ident "."] ident. */
// TODO

/* designator = qualident. */
// TODO

/* ProcedureHeading =
 *        "PROCEDURE" ident ["*"] FormalParameters */
// TODO

/* FormalParameters =
 *        "(" [FPSection {";" FPSection}] ")" ":"qualident */
// TODO

/* FPSection = ["VAR"] ident {"," ident} ":" FormalType */
// TODO

/* FormalType = qualident */
// TODO

/* ReturnStatement = "RETURN" [expression] */
// TODO

/* FunctionCall = designator "(" [ActualParameters] ")" */
// TODO

/* ActualParameters = expression {"," expression} */
// TODO

bool Parser::peek_check_token_type(TokenType tokenType, bool advanceOnTrue) {
  if (scanner_.peek()->type() == tokenType) {
    if (advanceOnTrue)
      curr_token_ = scanner_.next();

    return true;
  }

  return false;
}

bool Parser::peek_check_token_type_within(std::set<TokenType> expectedTypes,
                                          bool advanceOnTrue) {
  auto token = scanner_.peek();
  if (auto search = expectedTypes.find(token->type());
      search == expectedTypes.end()) {
    return false;
  }

  if (advanceOnTrue)
    curr_token_ = scanner_.next();

  return true;
}

bool Parser::expect_token_type(TokenType expectedType, bool advanceOnTrue,
                               bool advanceOnFalse) {
  auto token = scanner_.peek();
  if (token->type() != expectedType) {
    logger_.error(token->start(), to_string(expectedType) +
                                      " expected, found " +
                                      to_string(token->type()) + ".");

    if (advanceOnFalse)
      curr_token_ = scanner_.next();

    return false;
  }

  if (advanceOnTrue)
    curr_token_ = scanner_.next();

  return true;
}
