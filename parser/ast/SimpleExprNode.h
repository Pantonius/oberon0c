#ifndef OBERON0C_SIMPLEEXPRNODE_H
#define OBERON0C_SIMPLEEXPRNODE_H

#include "Node.h"
#include "RelationNode.h"
#include <memory>

const std::set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                              TokenType::op_minus};
enum SignType { s_plus, s_minus };

SignType sign_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return s_plus;
  case TokenType::op_minus:
    return s_minus;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

const std::set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};
enum AddOperatorType { a_plus, a_minus, a_or };

AddOperatorType add_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_plus:
    return a_plus;
  case TokenType::op_minus:
    return a_minus;
  case TokenType::op_or:
    return a_or;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

class SimpleExprNode final : public Node {
public:
  SimpleExprNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~SimpleExprNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_SIMPLEEXPRNODE_H
