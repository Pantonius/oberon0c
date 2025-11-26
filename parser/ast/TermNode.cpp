#include "TermNode.h"

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
