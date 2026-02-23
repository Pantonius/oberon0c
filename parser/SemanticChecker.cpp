#include "SemanticChecker.h"
#include "ast/DeclarationSequenceNode.h"
#include "ast/IdentNode.h"
#include "ast/ModuleNode.h"
#include "ast/TypeNode.h"
#include "global.h"
#include "parser/SymbolTable.h"
#include "parser/ast/ASTContext.h"
#include "util/Logger.h"
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

void SemanticChecker::onModuleStart(const FilePos pos,
                                    unique_ptr<IdentNode> ident) {
  auto module = make_unique<ModuleNode>(pos, std::move(ident));
  // TODO uniqueness check
  symbol_table_.beginScope();
  context_.set_module(std::move(module));
}

void SemanticChecker::onModuleEnd(const FilePos pos, const IdentNode &ident) {
  if (context_.get_module()->ident->value != ident.value) {
    logger_.error(pos, "End identifier does not match module identifier.");
    exit(EXIT_FAILURE);
  }

  symbol_table_.endScope();
}

unique_ptr<ConstDeclarationNode>
SemanticChecker::onConst(const FilePos pos, unique_ptr<IdentNode> ident,
                         unique_ptr<ExpressionNode> expr) {

  // TODO CHECK expr is constant? type of that constant?
  if (!expr) {
    logger_.error(pos, "undefined constant value: " + ident->value);
    exit(EXIT_FAILURE);
  }

  if (!expr->is_const()) {
    logger_.error(pos,
                  "Non-constant value in const declaration: " + ident->value);
    exit(EXIT_FAILURE);
  }

  auto type = expr->type;
  auto const_decl = make_unique<ConstDeclarationNode>(pos, std::move(ident),
                                                      std::move(expr), type);
  expect_unique(const_decl->ident.get(), const_decl.get());

  return const_decl;
}

vector<unique_ptr<VarDeclarationNode>>
SemanticChecker::onVars(const FilePos pos, vector<unique_ptr<IdentNode>> idents,
                        TypeNode *type) {
  if (!type) {
    logger_.error(pos, "Unspecified variable type.");
    exit(EXIT_FAILURE);
  }

  vector<unique_ptr<VarDeclarationNode>> var_decls;
  for (size_t i = 0; i < idents.size(); i++) {
    auto var_decl =
        make_unique<VarDeclarationNode>(pos, std::move(idents[i]), type);

    expect_unique(var_decl->ident.get(), var_decl.get());
    var_decls.push_back(std::move(var_decl));
  }

  return var_decls;
}

unique_ptr<TypeDeclarationNode> SemanticChecker::onTypeDeclaration(
    const FilePos pos, unique_ptr<IdentNode> ident, TypeNode *type) {
  auto type_declaration =
      make_unique<TypeDeclarationNode>(pos, std::move(ident), type);

  expect_unique(type_declaration->ident.get(), type_declaration.get());

  return type_declaration;
}

TypeNode *SemanticChecker::onIdentType(const FilePos pos,
                                       unique_ptr<IdentNode> ident) {
  try {
    auto std_type = ASTContext::std_types.at(ident->value);
    return std_type;
  } catch (...) {
  }

  auto type_decl_ = symbol_table_.lookup(*ident);

  if (!type_decl_) {
    logger_.error(pos, "Specified type '" + ident->value +
                           "' is not declared in the current scope.");
    return {};
  }

  if (auto type_decl =
          dynamic_cast<const TypeDeclarationNode *>(type_decl_.value())) {
    return type_decl->type;
  }

  logger_.error(pos, "'" + to_string(*ident) + "' is not a type.");
  return {};
}

ArrayTypeNode *SemanticChecker::onArrayType(const FilePos pos,
                                            unique_ptr<ExpressionNode> expr_,
                                            TypeNode *type) {
  if (!expr_) {
    logger_.error(pos, "Undefined array size.");
    exit(EXIT_FAILURE);
  }

  auto num = dynamic_unique_ptr_copy_cast<NumberExpressionNode>(expr_.get());

  if (!num) {
    logger_.error(num->pos(), "Array size is not a constant number.");
    exit(EXIT_FAILURE);
  }

  if (num->value < 0) {
    logger_.error(num->pos(), "Array size cannot be negative");
  }

  auto array_type = std::make_unique<ArrayTypeNode>(pos, std::move(num), type);
  auto ptr = context_.add_type(std::move(array_type));

  return ptr;
}

RecordTypeNode *SemanticChecker::onRecordType(
    const FilePos pos,
    vector<std::pair<vector<unique_ptr<IdentNode>>, TypeNode *>> fields) {

  symbol_table_.beginScope();

  vector<unique_ptr<VarDeclarationNode>> vars;
  for (size_t i = 0; i < fields.size(); i++) {
    auto decls = std::move(fields[i].first);
    auto type = fields[i].second;

    for (size_t j = 0; j < decls.size(); j++) {
      auto var_decl =
          make_unique<VarDeclarationNode>(pos, std::move(decls[j]), type);

      expect_unique_within_scope(var_decl->ident.get(), var_decl.get());
      vars.push_back(std::move(var_decl));
    }
  }

  symbol_table_.endScope();

  auto record_type = std::make_unique<RecordTypeNode>(pos, std::move(vars));
  auto ptr = context_.add_type(std::move(record_type));

  return ptr;
}

unique_ptr<ExpressionNode>
SemanticChecker::onIdentExpression(const FilePos pos,
                                   unique_ptr<IdentNode> ident,
                                   vector<unique_ptr<SelectorNode>> selectors) {

  // get ident type for more detailed error messages
  TypeNode *type;
  const DeclarationNode *decl;
  try {
    type = symbol_table_.lookup_type(*ident, selectors);
    decl = symbol_table_.lookup(*ident).value();
    if (!decl) {
      logger_.error(ident->pos(), to_string(*ident) + " is not a variable.");
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, type, false);
  }

  // lookup ident declaration
  auto node_lookup = symbol_table_.lookup(*ident);
  if (!node_lookup) {
    logger_.error(pos, "Undeclared identifier.");
    exit(EXIT_FAILURE);
  }

  auto node = node_lookup.value();

  if (node->getNodeType() == NodeType::const_declaration) {
    auto const_decl = dynamic_cast<const ConstDeclarationNode *>(node);

    auto expr = const_decl->expression.get();
    switch (expr->getNodeType()) {
    case NodeType::boolean:
      return clone_literal<BooleanExpressionNode>(
          dynamic_cast<BooleanExpressionNode *>(expr));
    case NodeType::number:
      return clone_literal<NumberExpressionNode>(
          dynamic_cast<NumberExpressionNode *>(expr));
    default:
      logger_.error(pos, "Constant of invalid node type.");
      exit(EXIT_FAILURE);
    }
  } else if (node->getNodeType() == NodeType::var_declaration ||
             node->getNodeType() == NodeType::param_declaration) {

    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, type, false);
  } else {
    logger_.error(pos, "Identifier is not a constant or variable.");
    exit(EXIT_FAILURE);
  }
}

ProcedureTypeNode *SemanticChecker::onProcedureType(
    const FilePos pos,
    vector<std::tuple<vector<unique_ptr<IdentNode>>, bool, TypeNode *>>
        formal_parameters) {
  symbol_table_.beginScope();

  vector<unique_ptr<ParamDeclarationNode>> params;
  for (size_t i = 0; i < formal_parameters.size(); i++) {
    auto idents = std::get<0>(std::move(formal_parameters[i]));
    auto by_reference = std::get<1>(formal_parameters[i]);
    auto type = std::get<2>(formal_parameters[i]);

    for (size_t j = 0; j < idents.size(); j++) {
      auto ident_pos = idents[j]->pos();
      auto param = std::make_unique<ParamDeclarationNode>(
          ident_pos, std::move(idents[j]), by_reference, type);

      expect_unique_within_scope(param->ident.get(), param.get());

      params.push_back(std::move(param));
    }
  }
  symbol_table_.endScope();

  auto proc_type = std::make_unique<ProcedureTypeNode>(pos, std::move(params));
  auto ptr = context_.add_type(std::move(proc_type));

  return ptr;
}

unique_ptr<ProcedureDeclarationNode> SemanticChecker::onProcedureDeclaration(
    const FilePos pos, unique_ptr<IdentNode> ident, ProcedureTypeNode *type) {
  auto proc_decl =
      std::make_unique<ProcedureDeclarationNode>(pos, std::move(ident), type);
  expect_unique(proc_decl->ident.get(), proc_decl.get());

  symbol_table_.beginScope();

  // introduce formal parameters into scope
  for (size_t i = 0; i < type->formal_parameters.size(); i++) {
    symbol_table_.insert(*type->formal_parameters[i]->ident,
                         type->formal_parameters[i].get());
  }

  return proc_decl;
}

void SemanticChecker::onProcedureEnd(const FilePos pos,
                                     const ProcedureDeclarationNode *procedure,
                                     const IdentNode &ident) {
  if (procedure->ident->value != ident.value) {
    logger_.error(pos, "End identifier does not match procedure identifier.");
    exit(EXIT_FAILURE);
  }

  // TODO checks
  symbol_table_.endScope();
}

unique_ptr<ProcedureCallNode> SemanticChecker::onProcedureCall(
    const FilePos pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<SelectorNode>> selectors,
    vector<unique_ptr<ExpressionNode>> actual_params) {

  // get procedure declaration
  auto opt_decl = symbol_table_.lookup(*ident);
  if (!opt_decl) {
    logger_.error(pos, "Undeclared procedure identifier.");
    exit(EXIT_FAILURE);
  }
  if (opt_decl.value()->getNodeType() != NodeType::procedure_declaration) {
    logger_.error(pos,
                  "Identifier is not associated with a procedure declaration.");
    exit(EXIT_FAILURE);
  }
  auto proc_decl =
      dynamic_cast<const ProcedureDeclarationNode *>(opt_decl.value());

  // look into procedure type
  auto proc_type = dynamic_cast<const ProcedureTypeNode *>(proc_decl->type);

  // match argument types
  if (proc_type->formal_parameters.size() != actual_params.size()) {
    logger_.error(pos, "Number of given parameters does not match declared "
                       "procedure parameters.");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < proc_type->formal_parameters.size(); i++) {
    // NOTE type compatibility for now just means identity
    // TODO add logic for type casting
    if (proc_type->formal_parameters[i]->type != actual_params[i]->type) {
      logger_.error(actual_params[i]->pos(),
                    "Parameter type does not match declared formal type.");
      exit(EXIT_FAILURE);
    }

    if (proc_type->formal_parameters[i]->by_reference) {
      if (actual_params[i]->getNodeType() != NodeType::ident_expression) {
        logger_.error(actual_params[i]->pos(),
                      "Passed parameter is not a variable.");
        exit(EXIT_FAILURE);
      }

      auto ident_expr =
          dynamic_cast<const IdentExpressionNode *>(actual_params[i].get());

      auto opt_decl = symbol_table_.lookup(*ident_expr->ident);
      if (!opt_decl ||
          opt_decl.value()->getNodeType() != NodeType::var_declaration) {
        logger_.error(actual_params[i]->pos(),
                      "Identifier is not associated with a var declaration.");
        exit(EXIT_FAILURE);
      }
    }
  }

  return std::make_unique<ProcedureCallNode>(
      pos, std::move(ident), std::move(selectors), std::move(actual_params));
}

unique_ptr<ExpressionNode> SemanticChecker::onUnaryExpression(
    const FilePos pos, unique_ptr<ExpressionNode> expr, const UnaryOpType op) {

  if (!expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  TypeNode *type;
  switch (op) {
  case UnaryOpType::u_not:
    expect_bool(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto boolean = dynamic_cast<BooleanExpressionNode *>(expr.get());

      int value = boolean->value;

      if (op == UnaryOpType::u_not)
        value = !boolean->value;

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  case UnaryOpType::plus:
  case UnaryOpType::minus:
    expect_number(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto number = dynamic_cast<NumberExpressionNode *>(expr.get());

      int value = number->value;

      if (op == UnaryOpType::minus)
        value = -number->value;

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::INTEGER;
    break;
  default:
    logger_.error(expr->pos(), "Unknown operation");
    exit(EXIT_FAILURE);
  }

  return std::make_unique<UnaryExpressionNode>(pos, op, std::move(expr), type);
}

unique_ptr<ExpressionNode> SemanticChecker::onBinaryExpression(
    const FilePos pos, unique_ptr<ExpressionNode> left_expr,
    const BinaryOpType op, unique_ptr<ExpressionNode> right_expr) {

  if (!left_expr || !right_expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  if (left_expr->type != right_expr->type) {
    logger_.error(right_expr->pos(), "Expression types do not match.");
    exit(EXIT_FAILURE);
  }

  // check type according to op
  TypeNode *type;
  switch (op) {
  case BinaryOpType::b_and:
  case BinaryOpType::b_or:
    expect_bool(left_expr.get());
    expect_bool(right_expr.get());

    // Literals
    if (left_expr->getNodeType() == NodeType::boolean &&
        right_expr->getNodeType() == NodeType::boolean) {
      auto left_bool = dynamic_cast<BooleanExpressionNode *>(left_expr.get());
      auto right_bool = dynamic_cast<BooleanExpressionNode *>(right_expr.get());

      bool value;
      if (op == BinaryOpType::b_and)
        value = left_bool->value & right_bool->value;
      else
        value = left_bool->value | right_bool->value;

      return std::make_unique<BooleanExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  case BinaryOpType::plus:
  case BinaryOpType::minus:
  case BinaryOpType::times:
  case BinaryOpType::div:
  case BinaryOpType::divide:
  case BinaryOpType::mod:
    expect_number(left_expr.get());
    expect_number(right_expr.get());

    // Literals
    if (left_expr->getNodeType() == NodeType::number &&
        right_expr->getNodeType() == NodeType::number) {
      auto left_number = dynamic_cast<NumberExpressionNode *>(left_expr.get());
      auto right_number =
          dynamic_cast<NumberExpressionNode *>(right_expr.get());

      int value;
      switch (op) {
      case BinaryOpType::plus:
        value = left_number->value + right_number->value;
        break;
      case BinaryOpType::minus:
        value = left_number->value - right_number->value;
        type = ASTContext::INTEGER;
        break;
      case BinaryOpType::times:
        value = left_number->value * right_number->value;
        break;
      case BinaryOpType::div:
      case BinaryOpType::divide:
        value = left_number->value / right_number->value;
        break;
      case BinaryOpType::mod:
        value = left_number->value % right_number->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::INTEGER;
    break;
  default:
    if (left_expr->getNodeType() == NodeType::number &&
        right_expr->getNodeType() == NodeType::number) {
      auto left_number = dynamic_cast<NumberExpressionNode *>(left_expr.get());
      auto right_number =
          dynamic_cast<NumberExpressionNode *>(right_expr.get());

      bool value;
      switch (op) {
      case BinaryOpType::eq:
        value = left_number->value == right_number->value;
        break;
      case BinaryOpType::neq:
        value = left_number->value != right_number->value;
        break;
      case BinaryOpType::lt:
        value = left_number->value < right_number->value;
        break;
      case BinaryOpType::leq:
        value = left_number->value <= right_number->value;
        break;
      case BinaryOpType::gt:
        value = left_number->value > right_number->value;
        break;
      case BinaryOpType::geq:
        value = left_number->value >= right_number->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }
      return std::make_unique<BooleanExpressionNode>(pos, value);
    } else if (left_expr->getNodeType() == NodeType::boolean &&
               right_expr->getNodeType() == NodeType::boolean) {
      auto left_boolean =
          dynamic_cast<BooleanExpressionNode *>(left_expr.get());
      auto right_boolean =
          dynamic_cast<BooleanExpressionNode *>(right_expr.get());

      bool value;
      switch (op) {
      case BinaryOpType::eq:
        value = left_boolean->value == right_boolean->value;
        break;
      case BinaryOpType::neq:
        value = left_boolean->value != right_boolean->value;
        break;
      case BinaryOpType::lt:
        value = left_boolean->value < right_boolean->value;
        break;
      case BinaryOpType::leq:
        value = left_boolean->value <= right_boolean->value;
        break;
      case BinaryOpType::gt:
        value = left_boolean->value > right_boolean->value;
        break;
      case BinaryOpType::geq:
        value = left_boolean->value >= right_boolean->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }
      return std::make_unique<BooleanExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  }

  return std::make_unique<BinaryExpressionNode>(pos, std::move(left_expr), op,
                                                std::move(right_expr), type);
}
unique_ptr<AssignmentNode>
SemanticChecker::onAssign(const FilePos pos, unique_ptr<IdentNode> ident,
                          vector<unique_ptr<SelectorNode>> selectors,
                          unique_ptr<ExpressionNode> expr) {
  TypeNode *lhs_type;
  const DeclarationNode *decl;
  try {
    lhs_type = symbol_table_.lookup_type(*ident, selectors);
    if (auto opt_decl = symbol_table_.lookup(*ident)) {
      if ((*opt_decl)->getNodeType() == NodeType::var_declaration ||
          (*opt_decl)->getNodeType() == NodeType::param_declaration) {
        decl = *opt_decl;
      } else {
        throw WrongNodeTypeException(
            *ident, "VarDeclarationNode / ParamDeclarationNode");
      }
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    auto ident_expr = std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, lhs_type, true);
    return std::make_unique<AssignmentNode>(pos, std::move(ident_expr),
                                            std::move(expr));
  }

  auto ident_expr = std::make_unique<IdentExpressionNode>(
      pos, std::move(ident), std::move(selectors), decl, lhs_type, true);
  if (ident_expr->type == nullptr) {
    logger_.error(pos, "'" + to_string(ident_expr.get()) +
                           "' has no associated type");
  } else if (expr->type == nullptr) {
    logger_.error(pos,
                  "'" + to_string(expr.get()) + "' has no associated type");
  } else if ((ident_expr->type != expr->type)) {
    logger_.error(pos, "Can not assign '" + to_string(expr.get()) + ": " +
                           to_string(expr->type) + "' to '" +
                           to_string(ident_expr.get()) + ": " +
                           to_string(ident_expr->type) + "'");
  }

  return std::make_unique<AssignmentNode>(pos, std::move(ident_expr),
                                          std::move(expr));
}

unique_ptr<ArrayIndexNode>
SemanticChecker::onArrayIndex(const FilePos pos,
                              unique_ptr<ExpressionNode> expr_) {

  if (expr_->type != ASTContext::INTEGER) {
    logger_.error(pos, "Array index is not an INTEGER");
  }
  if (auto number_expr =
          dynamic_unique_ptr_copy_cast<NumberExpressionNode>(expr_.get())) {

    if (number_expr->value < 0) {
      logger_.error(pos, "Array index cannot be negative");
    }
    return std::make_unique<ArrayIndexNode>(pos, std::move(number_expr));
  }

  return std::make_unique<ArrayIndexNode>(pos, std::move(expr_));
}

/* -------------------
 *  UTILITY FUNCTIONS
 * ------------------- */
void SemanticChecker::expect_unique(const IdentNode *ident,
                                    const DeclarationNode *value,
                                    bool this_scope) {
  if (auto decl = symbol_table_.lookup(*ident, this_scope)) {
    logger_.error(ident->pos(), "Identifier already declared here: " +
                                    to_string(decl.value()->pos()) + ":" +
                                    to_string(*decl));
    // exit(EXIT_FAILURE);
  }
  symbol_table_.insert(*ident, value);
  return;
}

void SemanticChecker::expect_unique_within_scope(const IdentNode *ident,
                                                 const DeclarationNode *value) {
  expect_unique(ident, value, true);
}

void SemanticChecker::expect_bool(ExpressionNode *expr) {
  if (expr->type != ASTContext::BOOLEAN) {
    logger_.error(expr->pos(), "Expression should be of type " +
                                   to_string(ASTContext::BOOLEAN) + ".");
    // exit(EXIT_FAILURE);
  }
}

void SemanticChecker::expect_number(ExpressionNode *expr) {
  if (expr->type != ASTContext::INTEGER) {
    logger_.error(expr->pos(), "Expression should be of type " +
                                   to_string(ASTContext::INTEGER) + ".");
    // exit(EXIT_FAILURE);
  }
}

template <typename L, typename T>
unique_ptr<LiteralExpressionNode<T>>
SemanticChecker::clone_literal(LiteralExpressionNode<T> *literal) {
  if (literal) {
    return std::make_unique<L>(literal->pos(), literal->value);
  }

  return nullptr;
}
