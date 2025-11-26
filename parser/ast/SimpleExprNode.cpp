#include "SimpleExprNode.h"

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
