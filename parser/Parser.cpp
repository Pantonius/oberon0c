#include "Parser.h"
#include "global.h"
#include "parser/ast/ExpressionNode.h"
#include "scanner/IdentToken.h"
#include "scanner/LiteralToken.h"
#include "scanner/Token.h"
#include <cstdlib>
#include <memory>
#include <vector>

using std::make_unique;

ASTContext *Parser::parse() {
  module();
  return sema_.get_context();
}

/* module = "MODULE" ident ";"
 *          DeclarationSequence
 *          [ "BEGIN" StatementSequence ]
 *          "END" ident "." */
void Parser::module() {
  if (!expect_token_type(TokenType::kw_module)) {
    logger_.info("You might be missing MODULE.");
  }
  const Token *curr = scanner_.peek();

  auto module_ident = Parser::ident();

  sema_.onModuleStart(curr->start(), std::move(module_ident));
  //
  expect_token_type(TokenType::semicolon);

  declaration_sequence(sema_.get_context()->get_module());

  // [
  unique_ptr<StatementSequenceNode> statement_sequence;
  if (peek_check_token_type(TokenType::kw_begin, ADVANCE_ON_TRUE)) {
    statement_sequence = Parser::statement_sequence();
  }
  // ]

  expect_token_type(TokenType::kw_end);
  module_ident = ident();
  expect_token_type(TokenType::period);
  expect_token_type(TokenType::eof);

  sema_.onModuleEnd(module_ident->pos(), std::move(module_ident));
}

/* ident = letter {letter | digit} */
unique_ptr<IdentNode> Parser::ident() {
  const Token *curr = scanner_.peek();
  if (expect_token_type(TokenType::const_ident)) {
    // Cast token pointer to IdentToken
    auto ident = dynamic_cast<const IdentToken *>(last_token_.get())->value();

    return make_unique<IdentNode>(curr->start(), ident);
  }

  exit(EXIT_FAILURE);
}

std::vector<unique_ptr<IdentNode>> Parser::ident_list() {
  std::vector<unique_ptr<IdentNode>> ident_list;

  do {
    ident_list.push_back(ident());
  } while (peek_check_token_type(TokenType::comma, true));

  return ident_list;
}

// type = ident | ArrayType | RecordType
const TypeNode *Parser::type() {
  const Token *curr = scanner_.peek();

  if (peek_ident()) {
    auto ident = Parser::ident();
    return sema_.onIdentType(curr->start(), std::move(ident));
  } else if (peek_array_type()) {
    return array_type();
  } else if (peek_record_type()) {
    return record_type();
  } else {
    logger_.error(curr->start(),
                  "Expected type found " + to_string(curr->type()));
    exit(EXIT_FAILURE);
  }
}

// ArrayType = "ARRAY" expression "OF" type
const ArrayTypeNode *Parser::array_type() {
  const Token *curr = scanner_.peek();

  expect_token_type(TokenType::kw_array);
  auto expression = Parser::expression();
  expect_token_type(TokenType::kw_of);
  auto type = Parser::type();

  return sema_.onArrayType(curr->start(), std::move(expression), type);
}

bool Parser::peek_array_type() {
  return peek_check_token_type(TokenType::kw_array);
}

// RecordType = "RECORD" FieldList {";" FieldList} "END"
const RecordTypeNode *Parser::record_type() {
  const Token *curr = scanner_.peek();

  expect_token_type(TokenType::kw_record);

  // symbol_table_.beginScope();

  vector<unique_ptr<VarDeclarationNode>> field_lists;
  do {
    vector<unique_ptr<IdentNode>> idents = ident_list();
    expect_token_type(TokenType::colon);
    const TypeNode *type = Parser::type();

    for (unique_ptr<IdentNode> &ident : idents) {
      auto field =
          make_unique<VarDeclarationNode>(ident->pos(), std::move(ident), type);
      field_lists.push_back(std::move(field));
    }
  } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));
  expect_token_type(TokenType::kw_end);

  return sema_.onRecordType(curr->start(), std::move(field_lists));
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
 *          [ "CONST" { ConstDeclaration ";" } ]
 *          [ "TYPE" { TypeDeclaration ";" } ]
 *          [ "VAR" { VarDeclaration ";" }]
 *          { ProcedureDeclaration ";" } */
void Parser::declaration_sequence(DeclarationSequenceNode *decls) {
  if (peek_check_token_type(TokenType::kw_const, ADVANCE_ON_TRUE)) {
    do {
      decls->add_const(const_declaration());
    } while (peek_const_declaration());
  }

  if (peek_check_token_type(TokenType::kw_type, ADVANCE_ON_TRUE)) {
    do {
      decls->add_type(type_declaration());
    } while (peek_type_declaration());
  }

  if (peek_check_token_type(TokenType::kw_var, ADVANCE_ON_TRUE)) {
    do {
      auto new_vars = var_declarations();
      for (size_t i = 0; i < new_vars.size(); i++) {
        decls->add_var(std::move(new_vars[i]));
      }
    } while (peek_var_declaration());
  }

  while (peek_check_token_type(TokenType::kw_procedure, NO_ADVANCE_ON_TRUE)) {
    decls->add_procedure(procedure_declaration());
  }
}

// ConstDeclaration = ident "=" expression ";"
unique_ptr<ConstDeclarationNode> Parser::const_declaration() {
  const Token *curr = scanner_.peek();

  auto ident = Parser::ident();
  expect_token_type(TokenType::op_eq);
  auto expression = Parser::expression();
  expect_token_type(TokenType::semicolon);

  return sema_.onConst(curr->start(), std::move(ident), std::move(expression));
}

bool Parser::peek_const_declaration() { return peek_ident(); }

// TypeDeclaration = ident "=" type ";"
std::unique_ptr<TypeDeclarationNode> Parser::type_declaration() {
  const Token *curr = scanner_.peek();

  auto ident = Parser::ident();
  expect_token_type(TokenType::op_eq);
  auto type = Parser::type();
  expect_token_type(TokenType::semicolon);

  auto type_declaration =
      sema_.onTypeDeclaration(curr->start(), std::move(ident), std::move(type));

  // auto type_declaration = make_unique<TypeDeclarationNode>(
  //     curr->start(), std::move(ident), type.get());
  // sema_.get_context()->add_type(std::move(type));
  // symbol_table_.insert(ident->value, type_declaration.get());

  return type_declaration;
}

bool Parser::peek_type_declaration() { return peek_ident(); }

// VarDeclaration = IdentList ":" type ";"
// IdentList = ident {"," ident}
vector<unique_ptr<VarDeclarationNode>> Parser::var_declarations() {
  auto curr_pos = last_token_->start();

  vector<unique_ptr<IdentNode>> idents = ident_list();
  expect_token_type(TokenType::colon);
  const TypeNode *type = Parser::type();

  auto var_declarations = sema_.onVars(curr_pos, std::move(idents), type);

  expect_token_type(TokenType::semicolon);

  return var_declarations;
}

bool Parser::peek_var_declaration() { return peek_ident(); }

// expression = SimpleExpr [relation SimpleExpr]
std::unique_ptr<ExpressionNode> Parser::expression() {
  const Token *curr = scanner_.peek();

  auto first_expr = simple_expr();
  if (!peek_check_token_type_within(RELATION_TOKEN_TYPES)) {
    return first_expr;
  }

  auto left_expression = std::move(first_expr);
  auto op = relation();
  auto right_expression = simple_expr();

  return make_unique<BinaryExpressionNode>(curr->start(),
                                           std::move(left_expression), op,
                                           std::move(right_expression),
                                           nullptr); // TODO sema type
}

bool Parser::peek_expression() { return peek_simple_expr(); }

// relation = "=" | "#" | "<" | "<=" | ">" | ">="
BinaryOpType Parser::relation() {
  expect_token_type_within(RELATION_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return relation_from_token_type(last_token_->type());
}

// SimpleExpr = ["+" | "-"] term {AddOperator term}
std::unique_ptr<ExpressionNode> Parser::simple_expr() {
  const Token *curr = scanner_.peek();

  unique_ptr<ExpressionNode> expr;
  if (peek_check_token_type_within(SIGN_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    // signed term
    auto op = sign();
    auto expression = term();

    expr = make_unique<UnaryExpressionNode>(curr->start(), op,
                                            std::move(expression),
                                            nullptr); // TODO sema type
  } else {
    // just term
    expr = term();
  }

  // add op
  while (peek_check_token_type_within(ADD_OPERATOR_TOKEN_TYPES,
                                      NO_ADVANCE_ON_TRUE)) {
    const Token *curr = scanner_.peek();

    auto left_expression = std::move(expr); // old expression
    auto op = add_operator();
    auto right_expression = term(); // new term
    expr = make_unique<BinaryExpressionNode>(curr->start(),
                                             std::move(left_expression), op,
                                             std::move(right_expression),
                                             nullptr); // TODO sema type
  }

  return expr;
}

bool Parser::peek_simple_expr() { return peek_sign() || peek_term(); }

UnaryOpType Parser::sign() {
  expect_token_type_within(SIGN_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return sign_from_token_type(last_token_->type());
}

bool Parser::peek_sign() {
  return peek_check_token_type_within(SIGN_TOKEN_TYPES).has_value();
}

// AddOperator = "+" | "-" | "OR"
BinaryOpType Parser::add_operator() {
  expect_token_type_within(ADD_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return add_operator_from_token_type(last_token_->type());
}

// term = factor {MulOperator factor}
std::unique_ptr<ExpressionNode> Parser::term() {
  const Token *curr = scanner_.peek();
  auto expr = factor();

  while (
      peek_check_token_type_within(MUL_OPERATOR_TOKEN_TYPES, NO_ADVANCE_ON_TRUE)
          .has_value()) {
    auto left_expression = std::move(expr);
    auto op = Parser::mul_operator();
    auto right_expression = Parser::factor();
    expr = make_unique<BinaryExpressionNode>(curr->start(),
                                             std::move(left_expression), op,
                                             std::move(right_expression),
                                             nullptr); // TODO sema type
  }

  return expr;
}

bool Parser::peek_term() { return peek_factor(); }

// MulOperator = "*" | "/" | "DIV" | "MOD" | "&"
BinaryOpType Parser::mul_operator() {
  expect_token_type_within(MUL_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return mul_operator_from_token_type(last_token_->type());
}
// factor = ident selector | number | "(" expression ")" | "~" factor
std::unique_ptr<ExpressionNode> Parser::factor() {
  const Token *curr = scanner_.peek();

  if (peek_ident()) {
    auto ident = Parser::ident();
    auto selectors = Parser::selectors();
    return make_unique<IdentExpressionNode>(curr->start(), std::move(ident),
                                            std::move(selectors)); // TODO type
  } else if (peek_number()) {
    auto number = Parser::number();
    return make_unique<NumberExpressionNode>(curr->start(), number,
                                             nullptr); // TODO type
  } else if (peek_check_token_type(TokenType::lparen, ADVANCE_ON_TRUE)) {
    auto expression = Parser::expression();
    expect_token_type(TokenType::rparen);
    return expression;
  } else if (peek_check_token_type(TokenType::op_not, ADVANCE_ON_TRUE)) {
    auto op = UnaryOpType::u_not;
    auto expression = Parser::expression();
    return make_unique<UnaryExpressionNode>(curr->start(), op,
                                            std::move(expression),
                                            nullptr); // TODO type
  } else {
    logger_.error(curr->start(),
                  "Expected factor found " + to_string(curr->type()));
    exit(EXIT_FAILURE);
  }
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

// ProcedureCall = ident selector ["(" ActualParameters ")"]
std::unique_ptr<ProcedureCallNode> Parser::procedure_call() {
  return Parser::procedure_call(ident());
}

std::unique_ptr<ProcedureCallNode>
Parser::procedure_call(unique_ptr<IdentNode> ident) {
  const Token *curr = scanner_.peek();

  auto selectors = Parser::selectors();

  vector<unique_ptr<ExpressionNode>> actual_parameters;
  if (peek_check_token_type(TokenType::lparen, ADVANCE_ON_TRUE)) {
    // actual parameters
    do {
      actual_parameters.push_back(expression());
    } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

    expect_token_type(TokenType::rparen);
  }

  return make_unique<ProcedureCallNode>(curr->start(), std::move(ident),
                                        selectors, actual_parameters);
}

// assignment = ident selector ":=" expression
std::unique_ptr<AssignmentNode> Parser::assignment() {
  return Parser::assignment(ident());
}

std::unique_ptr<AssignmentNode>
Parser::assignment(unique_ptr<IdentNode> ident) {
  const Token *curr = scanner_.peek();

  auto selectors = Parser::selectors();
  expect_token_type(TokenType::op_becomes);
  auto expression = Parser::expression();

  return make_unique<AssignmentNode>(curr->start(), std::move(ident),
                                     std::move(selectors),
                                     std::move(expression));
}

bool Parser::peek_ident() {
  return peek_check_token_type(TokenType::const_ident);
}

bool Parser::peek_assignment() { return peek_ident(); }
bool Parser::peek_assignment_without_ident() {
  return peek_selector() || peek_check_token_type(TokenType::op_becomes);
}
bool Parser::peek_selector() {
  return peek_check_token_type_within({TokenType::period, TokenType::lbrack})
      .has_value();
}

// StatementSequence = statement {";" statement}
unique_ptr<StatementSequenceNode> Parser::statement_sequence() {
  const Token *curr = scanner_.peek();

  vector<unique_ptr<StatementNode>> statement_sequence;
  do {
    statement_sequence.push_back(statement());
  } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));

  return make_unique<StatementSequenceNode>(curr->start(), statement_sequence);
}

// statement = [ assignment | ProcedureCall | IfStatement | WhileStatement ]
std::unique_ptr<StatementNode> Parser::statement() {
  if (peek_ident()) { // Could be assignment or procedure call
    auto ident = Parser::ident();
    if (peek_assignment_without_ident()) {
      return assignment(std::move(ident));
    } else {
      return procedure_call(std::move(ident));
    }
  } else if (peek_check_token_type(TokenType::kw_if)) {
    return if_statement();
  } else if (peek_check_token_type(TokenType::kw_while)) {
    return while_statement();
  } else {
    const Token *curr = scanner_.peek();
    logger_.error(curr->start(),
                  "Expected statement found " + to_string(curr->type()));
    exit(EXIT_FAILURE);
  }
}

// IfStatement = "IF" expression "THEN" StatementSequence {"ELSIF" expression
// "THEN" StatementSequence} ["ELSE" StatementSequence] "END"
std::unique_ptr<IfStatementNode> Parser::if_statement() {
  const Token *curr = scanner_.peek();

  expect_token_type(TokenType::kw_if);
  auto condition = expression();
  expect_token_type(TokenType::kw_then);
  auto body = statement_sequence();

  vector<unique_ptr<ElsIfStatementNode>> elsifs;
  while (peek_check_token_type(TokenType::kw_elsif, ADVANCE_ON_TRUE)) {
    const Token *local_curr = scanner_.peek();

    auto elsif_condition = Parser::expression();
    expect_token_type(TokenType::kw_then);
    auto elsif_body = statement_sequence();

    auto elsif = make_unique<ElsIfStatementNode>(
        local_curr->start(), std::move(elsif_condition), std::move(elsif_body));

    elsifs.push_back(std::move(elsif));
  }

  unique_ptr<StatementSequenceNode> else_statement_sequence;
  if (peek_check_token_type(TokenType::kw_else, ADVANCE_ON_TRUE)) {
    else_statement_sequence = statement_sequence();
  }

  expect_token_type(TokenType::kw_end);

  return make_unique<IfStatementNode>(curr->start(), std::move(condition),
                                      std::move(body), elsifs,
                                      std::move(else_statement_sequence));
}

// WhileStatement = "WHILE" expression "DO" StatementSequence "END"
std::unique_ptr<WhileStatementNode> Parser::while_statement() {
  const Token *curr = scanner_.peek();

  expect_token_type(TokenType::kw_while);
  auto condition = expression();
  expect_token_type(TokenType::kw_do);
  auto body = statement_sequence();

  expect_token_type(TokenType::kw_end);

  return make_unique<WhileStatementNode>(curr->start(), std::move(condition),
                                         std::move(body));
}

// RepeatStatement = "REPEAT" StatementSequence "UNTIL" expression
std::unique_ptr<RepeatStatementNode> Parser::repeat_statement() {
  const Token *curr = scanner_.peek();

  expect_token_type(TokenType::kw_repeat);
  auto body = statement_sequence();
  expect_token_type(TokenType::kw_until);
  auto condition = expression();

  return make_unique<RepeatStatementNode>(curr->start(), std::move(condition),
                                          std::move(body));
}

// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ";"
std::unique_ptr<ProcedureDeclarationNode> Parser::procedure_declaration() {
  const Token *curr = scanner_.peek();

  // ProcedureHeading = "PROCEDURE" ident [FormalParameters]
  expect_token_type(TokenType::kw_procedure);

  auto proc_name = ident();

  /* FormalParameters =
   *        "(" [FPSection {";" FPSection}] ")" */
  vector<unique_ptr<FPSectionNode>> formal_parameters;
  if (peek_check_token_type(TokenType::lparen, ADVANCE_ON_TRUE)) {
    if (!peek_check_token_type(TokenType::rparen, NO_ADVANCE_ON_TRUE)) {
      do {
        formal_parameters.push_back(fp_section());
      } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));
    }

    expect_token_type(TokenType::rparen);
  }
  expect_token_type(TokenType::semicolon);
  auto procedure_declaration = sema_.onProcedureStart(
      curr->start(), std::move(proc_name), std::move(formal_parameters));

  // ProcedureBody = DeclarationSequence ["BEGIN" StatementSequence] "END"
  // ident
  declaration_sequence(procedure_declaration.get());

  unique_ptr<StatementSequenceNode> statement_sequence;
  if (peek_check_token_type(TokenType::kw_begin, ADVANCE_ON_TRUE)) {
    procedure_declaration->set_statement_sequence(Parser::statement_sequence());
  }

  // END ident;
  expect_token_type(TokenType::kw_end);

  proc_name = ident();

  expect_token_type(TokenType::semicolon);

  // symbol_table_.endScope();
  sema_.onProcedureEnd(last_token_->start(), procedure_declaration.get(),
                       std::move(proc_name));

  return procedure_declaration;
}

/* FPSection = ["VAR"] ident {"," ident} ":" type */
std::unique_ptr<FPSectionNode> Parser::fp_section() {
  const Token *curr = scanner_.peek();

  peek_check_token_type(TokenType::kw_var,
                        ADVANCE_ON_TRUE); // NOTE optional without consequence

  vector<unique_ptr<IdentNode>> idents;
  do {
    idents.push_back(ident());
  } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  expect_token_type(TokenType::colon);

  auto type = Parser::type();

  return make_unique<FPSectionNode>(curr->start(), idents, type);
}

/* selector = {"." ident | "[" expression "]"} */
vector<unique_ptr<SelectorNode>> Parser::selectors() {
  vector<unique_ptr<SelectorNode>> selectors;
  while (auto token = peek_check_token_type_within(
             {TokenType::period, TokenType::lbrack}, ADVANCE_ON_TRUE)) {
    const Token *curr = scanner_.peek();

    switch (*token) {
    case TokenType::period: {
      auto ident = Parser::ident();
      selectors.push_back(
          make_unique<RecordFieldNode>(curr->start(), std::move(ident)));
      break;
    }
    case TokenType::lbrack: {
      auto expression = Parser::expression();
      selectors.push_back(
          make_unique<ArrayIndexNode>(curr->start(), std::move(expression)));
      expect_token_type(TokenType::rbrack);
      break;
    }
    default:
      logger_.error(last_token_->start(), "Expected selector found " +
                                              to_string(last_token_->type()));
      exit(EXIT_FAILURE);
    }
  }

  return selectors;
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
      const Token *curr = scanner_.peek();
      logger_.error(curr->start(),
                    "Expected number found " + to_string(curr->type()));
      break;
    }
  }

  exit(EXIT_FAILURE);
}

/* ===================
 *  UTILITY FUNCTIONS
 * =================== */
bool Parser::peek_check_token_type(TokenType tokenType, bool advanceOnTrue) {
  auto token = scanner_.peek();
  if (token->type() == tokenType) {
    if (advanceOnTrue) {
      last_token_ = scanner_.next();
    }

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

  if (advanceOnTrue) {
    last_token_ = scanner_.next();
  }

  return std::optional(token->type());
}

bool Parser::expect_token_type(TokenType expectedType, bool advanceOnTrue,
                               bool advanceOnFalse) {
  auto token = scanner_.peek();
  if (token->type() != expectedType) {
    logger_.error(token->start(), to_string(expectedType) +
                                      " expected, found " +
                                      to_string(token->type()) + ".");
    exit(EXIT_FAILURE);

    //   if (advanceOnFalse)
    //     last_token_ = scanner_.next();
    //
    //   return false;
  }

  if (advanceOnTrue) {
    last_token_ = scanner_.next();
  }

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
    exit(EXIT_FAILURE);
    // if (advanceOnFalse)
    //   last_token_ = scanner_.next();
    //
    // return false;
  }

  if (advanceOnTrue) {
    last_token_ = scanner_.next();
  }

  return true;
}
