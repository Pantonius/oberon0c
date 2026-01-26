#include "ExpressionNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

ostream &operator<<(ostream &os, UnaryOpType op) {
  switch (op) {
  case UnaryOpType::plus:
    return os << "+";
  case UnaryOpType::minus:
    return os << "-";
  case UnaryOpType::u_not:
    return os << "~";
  default:
    return os << "Unknown UnaryOpType";
  }
}

ostream &operator<<(ostream &os, BinaryOpType op) {
  switch (op) {
  case BinaryOpType::b_or:
    return os << "OR";
  case BinaryOpType::minus:
    return os << "-";
  case BinaryOpType::plus:
    return os << "+";
  case BinaryOpType::div:
    return os << "DIV";
  case BinaryOpType::divide:
    return os << "/";
  case BinaryOpType::b_and:
    return os << "AND";
  case BinaryOpType::mod:
    return os << "MOD";
  case BinaryOpType::times:
    return os << "*";
  case BinaryOpType::eq:
    return os << "=";
  case BinaryOpType::neq:
    return os << "#";
  case BinaryOpType::geq:
    return os << ">=";
  case BinaryOpType::gt:
    return os << ">";
  case BinaryOpType::leq:
    return os << "<=";
  case BinaryOpType::lt:
    return os << "<";
  default:
    return os << "Unknown BinaryOpType";
  }
}

BinaryOpType relation_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_eq:
    return BinaryOpType::eq;
  case TokenType::op_neq:
    return BinaryOpType::neq;
  case TokenType::op_lt:
    return BinaryOpType::lt;
  case TokenType::op_leq:
    return BinaryOpType::leq;
  case TokenType::op_gt:
    return BinaryOpType::gt;
  case TokenType::op_geq:
    return BinaryOpType::geq;
  default:
    exit(EXIT_FAILURE);
  }
}

UnaryOpType sign_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return UnaryOpType::plus;
  case TokenType::op_minus:
    return UnaryOpType::minus;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

BinaryOpType add_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return BinaryOpType::plus;
  case TokenType::op_minus:
    return BinaryOpType::minus;
  case TokenType::op_or:
    return BinaryOpType::b_or;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

BinaryOpType mul_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_times:
    return BinaryOpType::times;
  case TokenType::op_div:
    return BinaryOpType::div;
  case TokenType::op_divide:
    return BinaryOpType::divide;
  case TokenType::op_mod:
    return BinaryOpType::mod;
  case TokenType::op_and:
    return BinaryOpType::b_and;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

void ArrayIndexNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ArrayIndexNode::print(ostream &stream) const {
  stream << "[";
  expression->print(stream);
  stream << "]";
}

void RecordFieldNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void RecordFieldNode::print(ostream &stream) const {
  stream << ".";
  ident->print(stream);
}

void NumberExpressionNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void NumberExpressionNode::print(ostream &stream) const { stream << value; }

void BooleanExpressionNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void BooleanExpressionNode::print(ostream &stream) const { stream << value; }

void IdentExpressionNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IdentExpressionNode::print(ostream &stream) const {
  ident->print(stream);
  for (size_t i = 0; i < selectors.size(); i++) {
    selectors[i]->print(stream);
  }
}
bool IdentExpressionNode::is_const() const { return false; }

void UnaryExpressionNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void UnaryExpressionNode::print(ostream &stream) const {
  stream << op;
  expression->print(stream);
  stream << ";";
}
bool UnaryExpressionNode::is_const() const {
  return expression && expression->is_const();
}

void BinaryExpressionNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void BinaryExpressionNode::print(ostream &stream) const {
  left_expression->print(stream);
  stream << " " << op << " ";
  right_expression->print(stream);
}
bool BinaryExpressionNode::is_const() const {
  return left_expression && right_expression && left_expression->is_const() &&
         right_expression->is_const();
}
