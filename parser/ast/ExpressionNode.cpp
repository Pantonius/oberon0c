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
