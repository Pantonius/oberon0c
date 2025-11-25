#ifndef OBERON0C_ADDOPERATORNODE_H
#define OBERON0C_ADDOPERATORNODE_H

#include "Node.h"
#include "RelationNode.h"
#include <memory>

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

class AddOperatorNode final : public Node {
public:
  AddOperatorNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~AddOperatorNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_ADDOPERATORNODE_H
