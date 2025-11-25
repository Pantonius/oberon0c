#include "Parser.h"

std::unique_ptr<const ModuleNode> Parser::parse() { module(); }

/* ident = letter {letter | digit} */
const string Parser::ident() {
  if (expect_token_type(TokenType::char_literal)) {
    // Cast token pointer to IdentToken
    auto ident = dynamic_cast<const IdentToken *>(curr_token_.get());

    return ident->value();
  }
}

/* module = "MODULE" ident ";"
 *          [ ImportList ]
 *          DeclarationSequence
 *          [ "BEGIN" StatementSequence ]
 *          "END" ident "." */
const string Parser::type() {
  // TODO For now we will ignore this, though appendix A.6 Type declarations
  // gives an overview of what are valid (predefined) types
  return "";
}

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
/* NOTE I assume this should be:
 * DeclarationSequence = { CONST ... } {ProcedureDeclaration ";" }
 */
std::unique_ptr<DeclarationSequenceNode> Parser::declaration_sequence() {
  auto declarations =
      std::make_unique<DeclarationSequenceNode>(curr_token_->start());

  while (peek_check_token_type_within(
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
  const_declaration->expression = expression();

  return const_declaration;
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
  auto var_declarations = std::vector<std::unique_ptr<VarDeclarationNode>>();
  do {
    auto var_declaration =
        std::make_unique<VarDeclarationNode>(curr_token_->start());

    var_declaration->exported =
        peek_check_token_type(TokenType::op_times, ADVANCE_ON_TRUE);

    var_declarations.push_back(std::move(var_declaration));
  } while (expect_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  expect_token_type(TokenType::colon);

  auto var_type = type();

  for (const auto &var_declaration : var_declarations) {
    var_declaration->type = var_type;
  }

  return var_declarations;
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
RelationType Parser::relation() {
  expect_token_type_within(RELATION_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return relation_type_from_token_type(curr_token_->type());
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
    auto pair =
        std::make_unique<std::pair<AddOperatorType, std::unique_ptr<TermNode>>>(
            curr_token_->start());
    pair->first = add_operator();
    pair->second = term();
  }

  return simple_expr;
}

SignType Parser::sign() {
  expect_token_type_within(SIGN_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return sign_from_token_type(curr_token_->type());
}
// AddOperator = "+" | "-" | "OR"
AddOperatorType Parser::add_operator() {
  expect_token_type_within(ADD_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return add_operator_from_token_type(curr_token_->type());
}

// term = factor {MulOperator factor}
std::unique_ptr<TermNode> Parser::term() {
  auto term = std::make_unique<TermNode>(curr_token_->start());
  term->factor = factor();

  while (peek_check_token_type_within(MUL_OPERATOR_TOKEN_TYPES,
                                      NO_ADVANCE_ON_TRUE)) {
    auto pair = std::make_unique<
        std::pair<MulOperatorType, std::unique_ptr<FactorNode>>>(
        curr_token_->start());
    pair->first = mul_operator();
    pair->second = factor();
  }

  return term;
}
// MulOperator = "*" | "/" | "DIV" | "MOD" | "&"
MulOperatorType Parser::mul_operator() {
  expect_token_type_within(MUL_OPERATOR_TOKEN_TYPES, ADVANCE_ON_TRUE);
  return mul_operator_from_token_type(curr_token_->type());
}
// factor = number | CharConstant | string | set | NIL | designator |
// FunctionCall | "(" expression ")" | "~" factor
std::unique_ptr<FactorNode> Parser::factor() {
  auto factor = std::make_unique<FactorNode>(curr_token_->start());

  if (peek_check_token_type_within(
          {TokenType::int_literal, TokenType::float_literal,
           TokenType::double_literal, TokenType::byte_literal})) {
    // TODO
  } else if (peek_check_token_type(TokenType::char_literal)) {
    // TODO
  } else if (peek_check_token_type(TokenType::string_literal)) {
    // TODO
  } else if (peek_set()) {
    // TODO
  } else if (peek_check_token_type(TokenType::kw_nil)) {
    factor->nil = true; // TODO review
  } else if (peek_designator()) {
    // TODO
  } else if (peek_function_call()) {
    // TODO
  } else if (peek_check_token_type(TokenType::lparen, ADVANCE_ON_TRUE)) {
    factor->expression = expression();
    expect_token_type(TokenType::rparen);
  } else if (peek_check_token_type(TokenType::op_not, ADVANCE_ON_TRUE)) {
    factor->not_factor = Parser::factor();
  }

  return factor;
}

// FunctionCall = designator "(" [ActualParameters] ")"
std::unique_ptr<FunctionCallNode> Parser::function_call() {
  auto function_call = std::make_unique<FunctionCallNode>(curr_token_->start());

  function_call->designator = designator();

  expect_token_type(TokenType::lparen);

  if (!peek_check_token_type(TokenType::rparen)) {
    function_call->actual_parameters = actual_parameters();
  }

  expect_token_type(TokenType::rparen);

  return function_call;
}
// ActualParameters = expression {"," expression}
std::unique_ptr<ActualParametersNode> Parser::actual_parameters() {
  auto actual_parameters =
      std::make_unique<ActualParametersNode>(curr_token_->start());

  do {
    actual_parameters->expressions.push_back(expression());
  } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  return actual_parameters;
}

// designator = ident
std::unique_ptr<DesignatorNode> Parser::designator() {
  auto designator = std::make_unique<DesignatorNode>(curr_token_->start());

  designator->ident = ident();

  return designator;
}

// set = "{" [element {"," element}] "}"
std::unique_ptr<SetNode> Parser::set() {
  auto set = std::make_unique<SetNode>(curr_token_->start());

  expect_token_type(TokenType::lbrace);

  if (!peek_check_token_type(TokenType::rbrace)) {
    do {
      set->elements.push_back(element());
    } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));
  }

  expect_token_type(TokenType::rbrace);

  return set;
}

// element = expression [".." expression]
std::unique_ptr<ElementNode> Parser::element() {
  auto element = std::make_unique<ElementNode>(curr_token_->start());

  element->expression = expression();
  if (peek_check_token_type(TokenType::period, ADVANCE_ON_TRUE)) {
    expect_token_type(TokenType::period);
    element->until_expression = expression();
  }

  return element;
}

// assignment = designator ":=" expression
std::unique_ptr<AssignmentNode> Parser::assignment() {
  auto assignment = std::make_unique<AssignmentNode>(curr_token_->start());

  assignment->designator = designator();
  expect_token_type(TokenType::op_becomes);
  assignment->expression = expression();

  return assignment;
}

// StatementSequence = statement {";" statement}
std::unique_ptr<StatementSequenceNode> Parser::statement_sequence() {
  auto statement_sequence =
      std::make_unique<StatementSequenceNode>(curr_token_->start());

  do {
    statement_sequence->statements.push_back(statement());
  } while (peek_check_token_type(TokenType::semicolon));

  return statement_sequence;
}

// statement = [ assignment | ProcedureCall | IfStatement | CaseStatement |
// LoopStatement | WithStatement | ExitStatement | ReturnStatement ]
std::unique_ptr<StatementNode> Parser::statement() {
  auto statement = std::make_unique<StatementNode>(curr_token_->start());

  if (peek_assignment()) {

  } else if (peek_procedure_call()) {

  } else if (peek_if_statement()) {

  } else if (peek_case_statement()) {

  } else if (peek_loop_statement()) {

  } else if (peek_with_statement()) {

  } else if (peek_exit_statement()) {

  } else if (peek_return_statement()) {

  } else {
    // TODO throw tantrum
  }

  return statement;
}

// IfStatement = "IF" expression "THEN" StatementSequence {"ELSIF" expression
// "THEN" StatementSequence} ["ELSE" StatementSequence] "END"
std::unique_ptr<IfStatementNode> Parser::if_statement() {
  auto if_statement = std::make_unique<IfStatementNode>(curr_token_->start());

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

// CaseStatement = "CASE" expression "OF" clause {"|" clause} ["ELSE"
// StatementSequence] "END"
std::unique_ptr<CaseStatementNode> Parser::case_statement() {
  auto case_statement =
      std::make_unique<CaseStatementNode>(curr_token_->start());

  expect_token_type(TokenType::kw_case);
  case_statement->expression = expression();

  do {
    case_statement->clauses.push_back(clause());
  } while (peek_check_token_type(TokenType::pipe, ADVANCE_ON_TRUE));

  if (peek_check_token_type(TokenType::kw_else, ADVANCE_ON_TRUE)) {
    case_statement->else_statement_sequence = statement_sequence();
  }

  expect_token_type(TokenType::kw_end);

  return case_statement;
}
// clause = [CaseLabelList ":" StatementSequence]
std::unique_ptr<ClauseNode> Parser::clause() {
  auto clause = std::make_unique<ClauseNode>(curr_token_->start());

  if (case_label_list()) {
    clause->case_label_list = case_label_list();
    expect_token_type(TokenType::colon);
    clause->statement_sequence = statement_sequence();
  }

  return clause;
}

// CaseLabelList = CaseLabels {"," CaseLabels}
std::unique_ptr<CaseLabelListNode> Parser::case_label_list() {
  auto case_label_list =
      std::make_unique<CaseLabelListNode>(curr_token_->start());

  do {
    case_label_list->labels.push_back(case_labels());
  } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  return case_label_list;
}

// CaseLabels = ConstExpression [".." ConstExpression]
std::unique_ptr<CaseLabelsNode> Parser::case_labels() {
  auto case_labels = std::make_unique<CaseLabelsNode>(curr_token_->start());

  case_labels->const_expression = expression();
  if (peek_check_token_type(TokenType::period, ADVANCE_ON_TRUE)) {
    case_labels->until_const_expression = expression();
  }

  return case_labels;
}

// WhileStatement = "WHILE" expression "DO" StatementSequence "END"
std::unique_ptr<WhileStatementNode> Parser::while_statement() {
  auto while_statement =
      std::make_unique<WhileStatementNode>(curr_token_->start());

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
      std::make_unique<RepeatStatementNode>(curr_token_->start());

  expect_token_type(TokenType::kw_repeat);
  repeat_statement->body = statement_sequence();
  expect_token_type(TokenType::kw_until);
  repeat_statement->condition = expression();

  return repeat_statement;
}

// LoopStatement = "LOOP" StatementSequence "END"
std::unique_ptr<LoopStatementNode> Parser::loop_statement() {
  auto loop_statement =
      std::make_unique<LoopStatementNode>(curr_token_->start());

  expect_token_type(TokenType::kw_loop);
  loop_statement->body = statement_sequence();
  expect_token_type(TokenType::kw_end);

  return loop_statement;
}

// ExitStatement = "EXIT"
std::unique_ptr<ExitStatementNode> Parser::exit_statement() {
  auto exit_statement =
      std::make_unique<ExitStatementNode>(curr_token_->start());

  expect_token_type(TokenType::kw_exit);

  return exit_statement;
}

// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ident
std::unique_ptr<ProcedureDeclarationNode> Parser::procedure_declaration() {
  auto procedure_declaration =
      std::make_unique<ProcedureDeclarationNode>(curr_token_->start());

  procedure_declaration->heading = procedure_heading();
  expect_token_type(TokenType::semicolon);
  procedure_declaration->body = procedure_body();

  procedure_declaration->ident = ident();

  return procedure_declaration;
}
// ProcedureHeading = "PROCEDURE" ident ["*"] [FormalParameters]
std::unique_ptr<ProcedureHeadingNode> Parser::procedure_heading() {
  auto procedure_heading =
      std::make_unique<ProcedureHeadingNode>(curr_token_->start());

  expect_token_type(TokenType::kw_procedure);
  procedure_heading->ident = ident();

  if (peek_check_token_type(TokenType::op_times)) {
    procedure_heading->exported = true;
  }

  if (peek_formal_parameters()) {
    procedure_heading->formal_parameters = formal_parameters();
  }

  return procedure_heading;
}
// ProcedureBody = DeclarationSequence ["BEGIN" StatementSequence] "END"
std::unique_ptr<ProcedureBodyNode> Parser::procedure_body() {
  auto procedure_body =
      std::make_unique<ProcedureBodyNode>(curr_token_->start());

  procedure_body->declarations = declaration_sequence();

  if (peek_check_token_type(TokenType::kw_begin, ADVANCE_ON_TRUE)) {
    procedure_body->statement_sequence = statement_sequence();
  }

  expect_token_type(TokenType::kw_end);

  return procedure_body;
}

/* ImportList = "IMPORT" import {"," import} ";" */
std::unique_ptr<ImportListNode> Parser::import_list() {
  auto import_list = std::make_unique<ImportListNode>(curr_token_->start());

  expect_token_type(TokenType::kw_import);

  do {
    import_list->list.push_back(import());
  } while (peek_check_token_type(TokenType::comma, ADVANCE_ON_TRUE));

  expect_token_type(TokenType::semicolon);

  return import_list;
}

/* Import = ident [":=" ident] */
std::unique_ptr<ImportNode> Parser::import() {
  auto import = std::make_unique<ImportNode>(curr_token_->start());

  import->alias_ident = ident();

  if (peek_check_token_type(TokenType::op_becomes, ADVANCE_ON_TRUE)) {
    import->actual_ident = ident();
  }

  return import;
}

/* qualident = [ident "."] ident. */
std::unique_ptr<QualIdentNode> Parser::qual_ident() {
  auto qual_ident = std::make_unique<QualIdentNode>(curr_token_->start());

  string ident = Parser::ident();
  if (peek_check_token_type(TokenType::period, ADVANCE_ON_TRUE)) {
    qual_ident->module = ident;
    qual_ident->ident = Parser::ident();
    return qual_ident;
  }

  qual_ident->ident = ident;

  return qual_ident;
}

/* FormalParameters =
 *        "(" [FPSection {";" FPSection}] ")" ":"qualident */
std::unique_ptr<FormalParametersNode> Parser::formal_parameters() {
  auto formal_parameters =
      std::make_unique<FormalParametersNode>(curr_token_->start());

  expect_token_type(TokenType::lparen);

  // FPSection
  if (!peek_check_token_type(TokenType::rparen, NO_ADVANCE_ON_TRUE)) {
    do {
      formal_parameters->sections.push_back(fp_section());
    } while (peek_check_token_type(TokenType::semicolon, ADVANCE_ON_TRUE));
  }

  expect_token_type(TokenType::rparen);

  expect_token_type(TokenType::colon);

  formal_parameters->qual_ident = qual_ident();

  return formal_parameters;
}

/* FPSection = ["VAR"] ident {"," ident} ":" FormalType */
std::unique_ptr<FPSectionNode> Parser::fp_section() {
  auto fp_section = std::make_unique<FPSectionNode>(curr_token_->start());

  peek_check_token_type(TokenType::kw_var,
                        ADVANCE_ON_TRUE); // NOTE optional without consequence

  do {
    fp_section->idents.push_back(ident());
  } while (peek_check_token_type(TokenType::comma));

  expect_token_type(TokenType::colon);

  fp_section->type = formal_type();

  return fp_section;
}

/* FormalType = qualident */
std::unique_ptr<FormalTypeNode> Parser::formal_type() {
  auto formal_type = std::make_unique<FormalTypeNode>(curr_token_->start());

  formal_type->qual_ident = qual_ident();

  return formal_type;
}

/* ReturnStatement = "RETURN" [expression] */
std::unique_ptr<ReturnStatementNode> Parser::return_statement() {
  auto return_statement =
      std::make_unique<ReturnStatementNode>(curr_token_->start());

  expect_token_type(TokenType::kw_return);
  if (peek_expression()) {
    return_statement->expression = expression();
  }

  return return_statement;
}

/* ===================
 *  UTILITY FUNCTIONS
 * =================== */
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
