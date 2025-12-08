#include "Parser.h"
#include "global.h"
#include "scanner/IdentToken.h"
#include "scanner/LiteralToken.h"
#include "scanner/Token.h"
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<ModuleNode> Parser::parse() { return module(); }

/* module = "MODULE" ident ";"
 *          DeclarationSequence
 *          [ "BEGIN" StatementSequence ]
 *          "END" ident "." */
std::unique_ptr<ModuleNode> Parser::module() {
  if (!expect_token_type(TokenType::kw_module)) {

    logger_.info("You might be missing MODULE.");
  }

  auto module = std::make_unique<ModuleNode>(last_token_->start());

  module->ident = ident();
  expect_token_type(TokenType::semicolon);

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

/* ident = letter {letter | digit} */
string Parser::ident() {
  if (expect_token_type(TokenType::const_ident)) {
    // Cast token pointer to IdentToken
    auto ident = dynamic_cast<const IdentToken *>(last_token_.get());

    return ident->value();
  }

  exit(EXIT_FAILURE);
}

std::vector<string> Parser::ident_list() {
  std::vector<string> ident_list;

  do {
    ident_list.push_back(ident());
  } while (peek_check_token_type(TokenType::comma));

  return ident_list;
}

// type = ident | ArrayType | RecordType
std::unique_ptr<TypeNode> Parser::type() {
  std::unique_ptr<TypeNode> type =
      std::make_unique<TypeNode>(last_token_->start());
  if (peek_ident()) {
    type->ident = ident();
  } else if (peek_array_type()) {
    type->array_type = array_type();
  } else if (peek_array_type()) {
    type->record_type = record_type();
  } else {
    // TODO throw tentrum
  }

  return type;
}

// ArrayType = "ARRAY" expression "OF" type
std::unique_ptr<ArrayTypeNode> Parser::array_type() {
  std::unique_ptr<ArrayTypeNode> array_type =
      std::make_unique<ArrayTypeNode>(last_token_->start());

  expect_token_type(TokenType::kw_array);
  array_type->expression = expression();
  expect_token_type(TokenType::kw_of);
  array_type->type = type();

  return array_type;
}

bool Parser::peek_array_type() {
  return peek_check_token_type(TokenType::kw_array);
}

// RecordType = "RECORD" FieldList {";" FieldList} "END"
std::unique_ptr<RecordTypeNode> Parser::record_type() {
  std::unique_ptr<RecordTypeNode> record_type =
      std::make_unique<RecordTypeNode>(last_token_->start());

  expect_token_type(TokenType::kw_record);
  do {
    vector<string> idents = ident_list();
    expect_token_type(TokenType::colon);
    std::unique_ptr<TypeNode> type = Parser::type();

    // for (string ident : idents) {
    // record_type.field_lists.push_back(pair(ident, type));
    // }
  } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));
  expect_token_type(TokenType::kw_end);

  return record_type;
}

bool Parser::peek_record_type() {
  return peek_check_token_type(TokenType::kw_record);
}

// number = integer | real
// integer = digit {digit} | digit {hexDigit} "H"
// real = digit {digit} "." {digit} [ScaleFactor]
// ScaleFactor = ("E" | "D") ["+" | "-"] digit {digit}
// hexDigit = digit | "A" | "B" | "C" | "D" | "E" | "F"
// digit = "0" | ... | "9"

/* DeclarationSequence =
 *          [ "CONST" { ConstDeclaration ";" ]
 *          [ "TYPE" { TypeDeclaration ";" ]
 *          [ "VAR" { VarDeclaration ";" }]
 *          { ProcedureDeclaration ";" } */
std::unique_ptr<DeclarationSequenceNode> Parser::declaration_sequence() {
  auto declarations =
      std::make_unique<DeclarationSequenceNode>(last_token_->start());

  if (peek_check_token_type(TokenType::kw_const, ADVANCE_ON_TRUE)) {
    declarations->constants.push_back(const_declaration());
  }

  if (peek_check_token_type(TokenType::kw_type, ADVANCE_ON_TRUE)) {
    declarations->types.push_back(type_declaration());
  }

  if (peek_check_token_type(TokenType::kw_var, ADVANCE_ON_TRUE)) {
    var_declarations(declarations->variables);
  }

  while (peek_check_token_type(TokenType::kw_procedure, NO_ADVANCE_ON_TRUE)) {
    declarations->procedures.push_back(procedure_declaration());
  }

  return declarations;
}

// ConstDeclaration = ident "=" expression
std::unique_ptr<ConstDeclarationNode> Parser::const_declaration() {
  auto const_declaration =
      std::make_unique<ConstDeclarationNode>(last_token_->start());
  const_declaration->ident = ident();

  expect_token_type(TokenType::op_eq);

  const_declaration->expression = expression();

  return const_declaration;
}

// TypeDeclaration = ident "=" type
std::unique_ptr<TypeDeclarationNode> Parser::type_declaration() {
  auto type_declaration =
      std::make_unique<TypeDeclarationNode>(last_token_->start());

  type_declaration->ident = ident();
  expect_token_type(TokenType::op_eq);
  type_declaration->type = type();

  return type_declaration;
}

// VarDeclaration = IdentList ":" type
// IdentList = ident {"," ident}
void Parser::var_declarations(
    std::vector<unique_ptr<VarDeclarationNode>> &vars) {
  do {
    auto var_declaration =
        std::make_unique<VarDeclarationNode>(last_token_->start());

    vars.push_back(std::move(var_declaration));
  } while (expect_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  expect_token_type(TokenType::colon);

  auto var_type = type();

  // TODO
  // for (const auto &var_declaration : vars) {
  //   var_declaration->type = var_type;
  // }
}

// expression = SimpleExpr [relation SimpleExpr]
std::unique_ptr<ExpressionNode> Parser::expression() {
  auto expression = std::make_unique<ExpressionNode>(last_token_->start());
  expression->left_expr = simple_expr();

  if (peek_check_token_type_within(RELATION_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    expression->relation = relation();
    expression->right_expr = simple_expr();
  }

  return expression;
}

bool Parser::peek_expression() { return peek_simple_expr(); }

// relation = "=" | "#" | "<" | "<=" | ">" | ">=" | "IN" | "IS"
RelationType Parser::relation() {
  expect_token_type_within(RELATION_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return relation_from_token_type(last_token_->type());
}

// SimpleExpr = ["+" | "-"] term {AddOperator term}
std::unique_ptr<SimpleExprNode> Parser::simple_expr() {
  auto simple_expr = std::make_unique<SimpleExprNode>(last_token_->start());

  if (peek_check_token_type_within(SIGN_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    simple_expr->sign = sign();
  }

  simple_expr->term = term();

  while (
      peek_check_token_type_within(ADD_OPERATOR_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    simple_expr->additional_terms.push_back(std::pair(add_operator(), term()));
  }

  return simple_expr;
}

bool Parser::peek_simple_expr() { return peek_sign() || peek_term(); }

SignType Parser::sign() {
  expect_token_type_within(SIGN_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return sign_from_token_type(last_token_->type());
}

bool Parser::peek_sign() {
  return peek_check_token_type_within(SIGN_TOKEN_TYPES).has_value();
}

// AddOperator = "+" | "-" | "OR"
AddOperatorType Parser::add_operator() {
  expect_token_type_within(ADD_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return add_operator_from_token_type(last_token_->type());
}

// term = factor {MulOperator factor}
std::unique_ptr<TermNode> Parser::term() {
  auto term = std::make_unique<TermNode>(last_token_->start());
  term->factor = factor();

  while (
      peek_check_token_type_within(MUL_OPERATOR_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    logger_.debug("Parsing additional terms");
    auto mul_operator = Parser::mul_operator();
    auto factor = Parser::factor();
    term->additional_terms.emplace_back(mul_operator, std::move(factor));
  }

  return term;
}

bool Parser::peek_term() { return peek_factor(); }

// MulOperator = "*" | "/" | "DIV" | "MOD" | "&"
MulOperatorType Parser::mul_operator() {
  expect_token_type_within(MUL_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return mul_operator_from_token_type(last_token_->type());
}
// factor = ident selector | number | "(" expression ")" | "~" factor
std::unique_ptr<FactorNode> Parser::factor() {
  auto factor = std::make_unique<FactorNode>(last_token_->start());

  if (peek_ident()) {
    // TODO
    factor->ident = ident();
    factor->selector = selector();
  } else if (peek_number()) {
    // TODO
    factor->number = number();
  } else if (peek_check_token_type(TokenType::lparen, ADVANCE_ON_TRUE)) {
    factor->expression = expression();
    expect_token_type(TokenType::rparen);
  } else if (peek_check_token_type(TokenType::op_not, ADVANCE_ON_TRUE)) {
    factor->not_factor = Parser::factor();
  } else {
    logger_.debug("Could not parse factor");
  }

  return factor;
}

bool Parser::peek_factor() {
  return peek_ident() || peek_number() ||
         peek_check_token_type(TokenType::lparen) ||
         peek_check_token_type(TokenType::op_not);
}

bool Parser::peek_number(bool advanceOnTrue) {
  return peek_check_token_type_within(
             {
                 TokenType::short_literal,
                 TokenType::int_literal,
                 TokenType::long_literal,
             },
             advanceOnTrue)
      .has_value();
}

bool Parser::peek_char_constant() {
  return peek_check_token_type(TokenType::char_literal);
}

bool Parser::peek_string() {
  return peek_check_token_type(TokenType::string_literal);
}

// ProcedureCall = ident selector "(" [ActualParameters] ")"
std::unique_ptr<ProcedureCallNode> Parser::procedure_call() {
  return Parser::procedure_call(ident());
}

std::unique_ptr<ProcedureCallNode> Parser::procedure_call(string ident) {
  auto procedure_call =
      std::make_unique<ProcedureCallNode>(last_token_->start());

  procedure_call->ident = ident;
  procedure_call->selector = selector();

  expect_token_type(TokenType::lparen);

  if (!peek_check_token_type(TokenType::rparen)) {
    procedure_call->actual_parameters = actual_parameters();
  }

  expect_token_type(TokenType::rparen);

  return procedure_call;
}

bool Parser::peek_procedure_call_without_ident() {
  return peek_check_token_type(TokenType::lparen);
}

// ActualParameters = expression {"," expression}
std::unique_ptr<ActualParametersNode> Parser::actual_parameters() {
  auto actual_parameters =
      std::make_unique<ActualParametersNode>(last_token_->start());

  do {
    actual_parameters->expressions.push_back(expression());
  } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  return actual_parameters;
}

// assignment = ident selector ":=" expression
std::unique_ptr<AssignmentNode> Parser::assignment() {
  return Parser::assignment(ident());
}
std::unique_ptr<AssignmentNode> Parser::assignment(string ident) {
  auto assignment = std::make_unique<AssignmentNode>(last_token_->start());

  assignment->ident = ident;
  assignment->selector = selector();
  expect_token_type(TokenType::op_becomes);
  assignment->expression = expression();

  return assignment;
}

bool Parser::peek_ident() {
  return peek_check_token_type(TokenType::char_literal);
}

bool Parser::peek_assignment() { return peek_ident(); }
bool Parser::peek_assignment_without_ident() {
  return peek_check_token_type(TokenType::op_becomes, NO_ADVANCE_ON_TRUE);
}

// StatementSequence = statement {";" statement}
std::unique_ptr<StatementSequenceNode> Parser::statement_sequence() {
  auto statement_sequence =
      std::make_unique<StatementSequenceNode>(last_token_->start());

  do {
    statement_sequence->statements.push_back(statement());
  } while (peek_check_token_type(TokenType::semicolon));

  return statement_sequence;
}

// statement = [ assignment | ProcedureCall | IfStatement | WhileStatement ]
std::unique_ptr<StatementNode> Parser::statement() {
  auto statement = std::make_unique<StatementNode>(last_token_->start());

  if (peek_ident()) { // Could be assignment or procedure call
    string ident = Parser::ident();
    if (peek_assignment_without_ident()) {
      statement->assignment = assignment(ident);
    } else if (peek_procedure_call_without_ident()) {
      statement->procedure_call = procedure_call(ident);
    } else {
      // TODO throw tantrum
    }
  } else if (peek_check_token_type(TokenType::kw_if)) {
    statement->if_statement = if_statement();
  } else if (peek_check_token_type(TokenType::kw_while)) {
    statement->while_statement = while_statement();
  } else {
    // TODO throw tantrum
  }

  return statement;
}

// IfStatement = "IF" expression "THEN" StatementSequence {"ELSIF" expression
// "THEN" StatementSequence} ["ELSE" StatementSequence] "END"
std::unique_ptr<IfStatementNode> Parser::if_statement() {
  auto if_statement = std::make_unique<IfStatementNode>(last_token_->start());

  expect_token_type(TokenType::kw_if);
  if_statement->condition = expression();
  expect_token_type(TokenType::kw_then);
  if_statement->body = statement_sequence();

  while (peek_check_token_type(TokenType::kw_elsif, ADVANCE_ON_TRUE)) {
    auto expression = Parser::expression();
    expect_token_type(TokenType::kw_then);
    if_statement->elsifs.push_back(
        std::pair(std::move(expression), statement_sequence()));
  }

  if (peek_check_token_type(TokenType::kw_else, ADVANCE_ON_TRUE)) {
    if_statement->else_statement_sequence = statement_sequence();
  }

  expect_token_type(TokenType::kw_end);

  return if_statement;
}

// WhileStatement = "WHILE" expression "DO" StatementSequence "END"
std::unique_ptr<WhileStatementNode> Parser::while_statement() {
  auto while_statement =
      std::make_unique<WhileStatementNode>(last_token_->start());

  expect_token_type(TokenType::kw_while);
  while_statement->condition = expression();
  expect_token_type(TokenType::kw_do);

  while_statement->body = statement_sequence();

  expect_token_type(TokenType::kw_end);

  return while_statement;
}

// RepeatStatement = "REPEAT" StatementSequence "UNTIL" expression
std::unique_ptr<RepeatStatementNode> Parser::repeat_statement() {
  auto repeat_statement =
      std::make_unique<RepeatStatementNode>(last_token_->start());

  expect_token_type(TokenType::kw_repeat);
  repeat_statement->body = statement_sequence();
  expect_token_type(TokenType::kw_until);
  repeat_statement->condition = expression();

  return repeat_statement;
}

// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ident
std::unique_ptr<ProcedureDeclarationNode> Parser::procedure_declaration() {
  auto procedure_declaration =
      std::make_unique<ProcedureDeclarationNode>(last_token_->start());

  procedure_declaration->heading = procedure_heading();
  expect_token_type(TokenType::semicolon);
  procedure_declaration->body = procedure_body();

  procedure_declaration->ident = ident();

  return procedure_declaration;
}

// ProcedureHeading = "PROCEDURE" ident [FormalParameters]
std::unique_ptr<ProcedureHeadingNode> Parser::procedure_heading() {
  auto procedure_heading =
      std::make_unique<ProcedureHeadingNode>(last_token_->start());

  expect_token_type(TokenType::kw_procedure);
  procedure_heading->ident = ident();

  if (peek_formal_parameters()) {
    procedure_heading->formal_parameters = formal_parameters();
  }

  return procedure_heading;
}
// ProcedureBody = DeclarationSequence ["BEGIN" StatementSequence] "END" ident
std::unique_ptr<ProcedureBodyNode> Parser::procedure_body() {
  auto procedure_body =
      std::make_unique<ProcedureBodyNode>(last_token_->start());

  procedure_body->declarations = declaration_sequence();

  if (peek_check_token_type(TokenType::kw_begin, ADVANCE_ON_TRUE)) {
    procedure_body->statement_sequence = statement_sequence();
  }

  expect_token_type(TokenType::kw_end);

  procedure_body->end_ident = ident();

  return procedure_body;
}

/* FormalParameters =
 *        "(" [FPSection {";" FPSection}] ")" */
std::unique_ptr<FormalParametersNode> Parser::formal_parameters() {
  auto formal_parameters =
      std::make_unique<FormalParametersNode>(last_token_->start());

  expect_token_type(TokenType::lparen);

  // FPSection
  if (!peek_check_token_type(TokenType::rparen, NO_ADVANCE_ON_TRUE)) {
    do {
      formal_parameters->sections.push_back(fp_section());
    } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));
  }

  expect_token_type(TokenType::rparen);

  return formal_parameters;
}

bool Parser::peek_formal_parameters() {
  return peek_check_token_type(TokenType::lparen);
}

/* FPSection = ["VAR"] ident {"," ident} ":" type */
std::unique_ptr<FPSectionNode> Parser::fp_section() {
  auto fp_section = std::make_unique<FPSectionNode>(last_token_->start());

  peek_check_token_type(TokenType::kw_var,
                        ADVANCE_ON_TRUE); // NOTE optional without consequence

  do {
    fp_section->idents.push_back(ident());
  } while (peek_check_token_type(TokenType::comma));

  expect_token_type(TokenType::colon);

  fp_section->type = type();

  return fp_section;
}

/* selector = {"." ident | "[" expression "]"} */
std::unique_ptr<SelectorNode> Parser::selector() {
  auto selector = std::make_unique<SelectorNode>(last_token_->start());

  while (auto token = peek_check_token_type_within(
             {TokenType::period, TokenType::lbrack}, ADVANCE_ON_TRUE)) {

    switch (*token) {
    case TokenType::period:
      selector->ident = ident();
    case TokenType::lbrack:
      selector->expression = expression();
      expect_token_type(TokenType::rbrack);
    }
  }

  return selector;
}

/* number = integer */
int Parser::number() {
  if (peek_number(true)) {
    // Cast token pointer to IdentToken

    switch (last_token_->type()) {
    case TokenType::short_literal:
      return dynamic_cast<const ShortLiteralToken *>(last_token_.get())
          ->value();
    case TokenType::int_literal:
      return dynamic_cast<const IntLiteralToken *>(last_token_.get())->value();
    case TokenType::long_literal:
      return dynamic_cast<const LongLiteralToken *>(last_token_.get())->value();
    default:
      break;
    }
  }

  exit(EXIT_FAILURE);
}

/* ===================
 *  UTILITY FUNCTIONS
 * =================== */
bool Parser::peek_check_token_type(TokenType tokenType, bool advanceOnTrue) {
  if (scanner_.peek()->type() == tokenType) {
    if (advanceOnTrue)
      last_token_ = scanner_.next();

    return true;
  }

  return false;
}

std::optional<TokenType>
Parser::peek_check_token_type_within(std::set<TokenType> expectedTypes,
                                     bool advanceOnTrue) {
  auto token = scanner_.peek();
  if (auto search = expectedTypes.find(token->type());
      search == expectedTypes.end()) {
    return std::nullopt;
  }

  if (advanceOnTrue)
    last_token_ = scanner_.next();

  return std::optional(token->type());
}

bool Parser::expect_token_type(TokenType expectedType, bool advanceOnTrue,
                               bool advanceOnFalse) {
  auto token = scanner_.peek();
  if (token->type() != expectedType) {
    logger_.error(token->start(), to_string(expectedType) +
                                      " expected, found " +
                                      to_string(token->type()) + ".");

    if (advanceOnFalse)
      last_token_ = scanner_.next();

    return false;
  }

  if (advanceOnTrue)
    last_token_ = scanner_.next();

  return true;
}

bool Parser::expect_token_type_within(std::set<TokenType> expectedTypes,
                                      bool advanceOnTrue, bool advanceOnFalse) {
  auto token = scanner_.peek();
  if (!expectedTypes.contains(token->type())) {
    std::string s_expectedTypes = "{ ";
    for (auto type : expectedTypes) {
      s_expectedTypes.append(to_string(type) + ", ");
    }
    s_expectedTypes.append("}");

    logger_.error(token->start(), "one of " + s_expectedTypes +
                                      " expected, found " +
                                      to_string(token->type()) + ".");

    if (advanceOnFalse)
      last_token_ = scanner_.next();

    return false;
  }

  if (advanceOnTrue)
    last_token_ = scanner_.next();

  return true;
}
