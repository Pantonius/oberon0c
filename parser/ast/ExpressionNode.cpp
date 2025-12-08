#include "ExpressionNode.h"

RelationType relation_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_eq:
    return RelationType::eq;
  case TokenType::op_neq:
    return RelationType::neq;
  case TokenType::op_lt:
    return RelationType::lt;
  case TokenType::op_leq:
    return RelationType::leq;
  case TokenType::op_gt:
    return RelationType::gt;
  case TokenType::op_geq:
    return RelationType::geq;
  default:
    exit(EXIT_FAILURE);
  }
}

SignType sign_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return SignType::plus;
  case TokenType::op_minus:
    return SignType::minus;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

AddOperatorType add_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return AddOperatorType::plus;
  case TokenType::op_minus:
    return AddOperatorType::minus;
  case TokenType::op_or:
    return AddOperatorType::a_or;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

MulOperatorType mul_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_times:
    return MulOperatorType::times;
  case TokenType::op_div:
    return MulOperatorType::div;
  case TokenType::op_divide:
    return MulOperatorType::divide;
  case TokenType::op_mod:
    return MulOperatorType::mod;
  case TokenType::op_and:
    return MulOperatorType::m_and;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}
