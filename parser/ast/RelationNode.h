#ifndef OBERON0C_RELATIONNODE_H
#define OBERON0C_RELATIONNODE_H

#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <set>

const std::set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq, TokenType::op_neq, TokenType::op_lt, TokenType::op_leq,
    TokenType::op_gt, TokenType::op_geq, TokenType::op_in, TokenType::op_is};

enum RelationType { eq, neq, lt, leq, gt, geq, in, is };

RelationType relation_type_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_eq:
    return eq;
  case TokenType::op_neq:
    return neq;
  case TokenType::op_lt:
    return lt;
  case TokenType::op_leq:
    return leq;
  case TokenType::op_gt:
    return gt;
  case TokenType::op_geq:
    return geq;
  case TokenType::op_in:
    return in;
  case TokenType::op_is:
    return is;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

class RelationNode final : public Node {
public:
  RelationNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~RelationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  RelationType type;
};

#endif // OBERON0C_RELATIONNODE_H
