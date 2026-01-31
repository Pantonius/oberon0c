#include "SemanticChecker.h"
#include "ast/IdentNode.h"
#include "ast/ModuleNode.h"
#include "global.h"
#include "parser/SymbolTable.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/TypeNode.h"
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

void SemanticChecker::onModuleStart(const FilePos &pos,
                                    unique_ptr<IdentNode> ident) {
  auto module = make_unique<ModuleNode>(pos, std::move(ident));
  // TODO uniqueness check
  symbol_table_.beginScope();
  context_.set_module(std::move(module));
}

void SemanticChecker::onModuleEnd(const FilePos &pos,
                                  const unique_ptr<IdentNode> &ident) {
  if (context_.get_module()->ident->value != ident->value) {
    logger_.error(pos, "End identifier does not match module identifier.");
    exit(EXIT_FAILURE);
  }

  symbol_table_.endScope();
}

unique_ptr<ConstDeclarationNode>
SemanticChecker::onConst(const FilePos &pos, unique_ptr<IdentNode> ident,
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
SemanticChecker::onVars(const FilePos &pos,
                        vector<unique_ptr<IdentNode>> idents,
                        const TypeNode *type) {
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
    const FilePos &pos, unique_ptr<IdentNode> ident, const TypeNode *type) {
  auto type_declaration =
      make_unique<TypeDeclarationNode>(pos, std::move(ident), type);

  expect_unique(type_declaration->ident.get(), type_declaration.get());

  return type_declaration;
}

const TypeNode *SemanticChecker::onIdentType(const FilePos &pos,
                                             unique_ptr<IdentNode> ident) {
  if (ident->value == ASTContext::INTEGER.get_name())
    return ASTContext::INTEGER.get();
  if (ident->value == context_.BOOLEAN.get_name())
    return context_.BOOLEAN.get();

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

const ArrayTypeNode *
SemanticChecker::onArrayType(const FilePos &pos,
                             unique_ptr<ExpressionNode> expr_,
                             const TypeNode *type) {
  if (!expr_) {
    logger_.error(pos, "Undefined array size.");
    exit(EXIT_FAILURE);
  }

  std::unique_ptr<NumberExpressionNode> expr =
      std::unique_ptr<NumberExpressionNode>(
          dynamic_cast<NumberExpressionNode *>(expr_.release()));

  if (!expr) {
    logger_.error(expr->pos(), "Array size is not a constant number.");
    exit(EXIT_FAILURE);
  }

  if (expr->value < 0) {
    logger_.error(expr->pos(), "Array size cannot be negative");
  }

  auto array_type = std::make_unique<ArrayTypeNode>(pos, std::move(expr), type);
  auto ptr = context_.add_type(std::move(array_type));

  return ptr;
}

const RecordTypeNode *
SemanticChecker::onRecordType(const FilePos &pos,
                              vector<unique_ptr<VarDeclarationNode>> fields) {
  auto record_type = std::make_unique<RecordTypeNode>(pos, std::move(fields));
  // TODO check fields?

  auto ptr = context_.add_type(std::move(record_type));

  return ptr;
}

unique_ptr<ExpressionNode>
SemanticChecker::onIdentExpression(const FilePos &pos,
                                   unique_ptr<IdentNode> ident,
                                   vector<unique_ptr<SelectorNode>> selectors) {
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
  } else if (node->getNodeType() == NodeType::var_declaration) {
    auto var_decl = dynamic_cast<const VarDeclarationNode *>(node);
    auto type = symbol_table_.lookup_type(*ident, selectors);

    return std::make_unique<IdentExpressionNode>(pos, std::move(ident),
                                                 std::move(selectors), type);
  } else {
    logger_.error(pos, "Identifier is not a constant or variable.");
    exit(EXIT_FAILURE);
  }

  // const TypeNode *type = decl->type;
  // switch (decl->getNodeType()) {
  // case NodeType::var_declaration:
  //   // TODO value lookup
  //   break;
  // case NodeType::const_declaration:
  //   // TODO value lookup
  //   break;
  // case NodeType::type_declaration:
  //   logger_.error(pos, "Types can not be used as expressions.");
  //   exit(EXIT_FAILURE);
  // case NodeType::procedure_declaration:
  //   logger_.error(pos, "Procedure identifiers can not be used as
  //   expressions."); exit(EXIT_FAILURE);
  // default:
  //   logger_.error(decl->pos(), "Unknown declaration node type.");
  //   exit(EXIT_FAILURE);
  // }
  //
  // return std::make_unique<IdentExpressionNode>(pos, std::move(ident),
  //                                              std::move(selectors), type);
}

unique_ptr<ProcedureDeclarationNode> SemanticChecker::onProcedureStart(
    const FilePos &pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<FPSectionNode>> formal_parameters) {
  auto proc = make_unique<ProcedureDeclarationNode>(
      pos, std::move(ident), std::move(formal_parameters),
      ASTContext::STD_PROCEDURE_TYPE.get());
  expect_unique(proc->ident.get(), proc.get());

  symbol_table_.beginScope();

  // TODO checks

  return proc;
}

void SemanticChecker::onProcedureEnd(const FilePos &pos,
                                     const ProcedureDeclarationNode *procedure,
                                     const unique_ptr<IdentNode> &ident) {
  if (procedure->ident->value != ident->value) {
    logger_.error(pos, "End identifier does not match procedure identifier.");
    exit(EXIT_FAILURE);
  }

  // TODO checks
  symbol_table_.endScope();
}

unique_ptr<ExpressionNode> SemanticChecker::onUnaryExpression(
    const FilePos &pos, unique_ptr<ExpressionNode> expr, UnaryOpType op) {

  if (!expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  const TypeNode *type;
  switch (op) {
  case UnaryOpType::u_not:
    expect_bool(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto boolean = dynamic_cast<BooleanExpressionNode *>(expr.get());

      int value = boolean->value;

      if (op == UnaryOpType::u_not)
        value = !boolean->value;

      return std::make_unique<NumberExpressionNode>(pos, value,
                                                    ASTContext::INTEGER.get());
    }

    type = ASTContext::BOOLEAN.get();
    break;
  case UnaryOpType::plus:
  case UnaryOpType::minus:
    expect_number(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto number = dynamic_cast<NumberExpressionNode *>(expr.get());

      int value = number->value;

      if (op == UnaryOpType::minus)
        value = -number->value;

      return std::make_unique<NumberExpressionNode>(pos, value,
                                                    ASTContext::INTEGER.get());
    }

    type = ASTContext::INTEGER.get();
    break;
  default:
    logger_.error(expr->pos(), "Unknown operation");
    exit(EXIT_FAILURE);
  }

  return std::make_unique<UnaryExpressionNode>(pos, op, std::move(expr), type);
}

unique_ptr<ExpressionNode> SemanticChecker::onBinaryExpression(
    const FilePos &pos, unique_ptr<ExpressionNode> left_expr, BinaryOpType op,
    unique_ptr<ExpressionNode> right_expr) {

  if (!left_expr || !right_expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  // check type according to op
  const TypeNode *type;
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

      return std::make_unique<BooleanExpressionNode>(pos, value,
                                                     ASTContext::BOOLEAN.get());
    }

    type = ASTContext::BOOLEAN.get();
    break;
  case BinaryOpType::plus:
  case BinaryOpType::minus:
  case BinaryOpType::times:
  case BinaryOpType::div:
  case BinaryOpType::divide:
  case BinaryOpType::mod:
  case BinaryOpType::gt:
  case BinaryOpType::geq:
  case BinaryOpType::lt:
  case BinaryOpType::leq:
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
      case BinaryOpType::gt:
        value = left_number->value > right_number->value;
        break;
      case BinaryOpType::geq:
        value = left_number->value >= right_number->value;
        break;
      case BinaryOpType::lt:
        value = left_number->value < right_number->value;
        break;
      case BinaryOpType::leq:
        value = left_number->value <= right_number->value;
        break;
      }

      return std::make_unique<NumberExpressionNode>(pos, value,
                                                    ASTContext::INTEGER.get());
    }

    type = ASTContext::INTEGER.get();
    break;
  default:
    if (left_expr->type != right_expr->type) {
      logger_.error(right_expr->pos(), "Expression types do not match.");
      exit(EXIT_FAILURE);
    }

    if (left_expr->getNodeType() == NodeType::number &&
        right_expr->getNodeType() == NodeType::number) {
      auto left_number = dynamic_cast<NumberExpressionNode *>(left_expr.get());
      auto right_number =
          dynamic_cast<NumberExpressionNode *>(right_expr.get());

      bool value;
      if (op == BinaryOpType::eq) {
        value = left_number->value == right_number->value;
      } else {
        value = left_number->value != right_number->value;
      }
      return std::make_unique<BooleanExpressionNode>(pos, value,
                                                     ASTContext::BOOLEAN.get());
    } else if (left_expr->getNodeType() == NodeType::boolean &&
               right_expr->getNodeType() == NodeType::boolean) {
      auto left_boolean =
          dynamic_cast<BooleanExpressionNode *>(left_expr.get());
      auto right_boolean =
          dynamic_cast<BooleanExpressionNode *>(right_expr.get());

      bool value;
      if (op == BinaryOpType::eq) {
        value = left_boolean->value == right_boolean->value;
      } else {
        value = left_boolean->value != right_boolean->value;
      }
      return std::make_unique<BooleanExpressionNode>(pos, value,
                                                     ASTContext::BOOLEAN.get());
    }

    type = left_expr->type;
    break;
  }

  return std::make_unique<BinaryExpressionNode>(pos, std::move(left_expr), op,
                                                std::move(right_expr), type);
}
unique_ptr<AssignmentNode>
SemanticChecker::onAssign(const FilePos &pos, unique_ptr<IdentNode> ident,
                          vector<unique_ptr<SelectorNode>> selectors,
                          unique_ptr<ExpressionNode> expr) {
  const TypeNode *lhs_type;
  try {
    lhs_type = symbol_table_.lookup_type(*ident, selectors);
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    auto ident_expr = std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), lhs_type);
    return std::make_unique<AssignmentNode>(pos, std::move(ident_expr),
                                            std::move(expr));
  }

  auto ident_expr = std::make_unique<IdentExpressionNode>(
      pos, std::move(ident), std::move(selectors), lhs_type);
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
SemanticChecker::onArrayIndex(const FilePos &pos,
                              unique_ptr<ExpressionNode> expr_) {
  auto expr = std::unique_ptr<NumberExpressionNode>(
      dynamic_cast<NumberExpressionNode *>(expr_.release()));

  if (!expr) {
    logger_.error(pos, "Array index is not a constant number");
    exit(EXIT_FAILURE);
  }

  if (expr->value < 0) {
    logger_.error(pos, "Array index cannot be negative");
  }

  return std::make_unique<ArrayIndexNode>(pos, std::move(expr));
}

/* -------------------
 *  UTILITY FUNCTIONS
 * ------------------- */
void SemanticChecker::expect_unique(const IdentNode *ident,
                                    const DeclarationNode *value) {
  if (auto decl = symbol_table_.lookup(*ident)) {
    logger_.error(ident->pos(), "Identifier already declared here: " +
                                    to_string(decl.value()->pos()) + ":" +
                                    to_string(*decl));
    // exit(EXIT_FAILURE);
  }
  symbol_table_.insert(*ident, value);
  return;
}

void SemanticChecker::expect_bool(ExpressionNode *expr) {
  if (expr->type != ASTContext::BOOLEAN.get()) {
    logger_.error(expr->pos(), "Expression should be of type " +
                                   ASTContext::BOOLEAN.get_name() + ".");
    // exit(EXIT_FAILURE);
  }
}

void SemanticChecker::expect_number(ExpressionNode *expr) {
  if (expr->type != ASTContext::INTEGER.get()) {
    // expr->print(cout);
    // expr->type->print(cout);
    logger_.error(expr->pos(), "Expression should be of type " +
                                   ASTContext::INTEGER.get_name() + ".");
    // exit(EXIT_FAILURE);
  }
}

template <typename L, typename T>
unique_ptr<LiteralExpressionNode<T>>
SemanticChecker::clone_literal(LiteralExpressionNode<T> *literal) {
  if (literal) {
    return std::make_unique<L>(literal->pos(), literal->value, literal->type);
  }

  return nullptr;
}
