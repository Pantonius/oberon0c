#ifndef OBERON0C_SIMPLEEXPRNODE_H
#define OBERON0C_SIMPLEEXPRNODE_H

#include "Node.h"
#include "RelationNode.h"
#include "TermNode.h"
#include <memory>
#include <utility>
#include <vector>

const std::set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                              TokenType::op_minus};
enum class SignType { plus, minus };

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

const std::set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};
enum class AddOperatorType { plus, minus, a_or };

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

class SimpleExprNode final : public Node {
public:
  SimpleExprNode(const FilePos &pos) : Node(NodeType::simple_expr, pos) {}
  ~SimpleExprNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  SignType sign;
  std::unique_ptr<TermNode> term;
  std::vector<std::pair<AddOperatorType, std::unique_ptr<TermNode>>>
      additional_terms;
};

#endif // OBERON0C_SIMPLEEXPRNODE_H
