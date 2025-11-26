#ifndef OBERON0C_RELATIONNODE_H
#define OBERON0C_RELATIONNODE_H

#include "Node.h"
#include "scanner/Token.h"
#include <set>

using std::set;

const set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq, TokenType::op_neq, TokenType::op_lt, TokenType::op_leq,
    TokenType::op_gt, TokenType::op_geq, TokenType::op_in, TokenType::op_is};

enum class RelationType { eq, neq, lt, leq, gt, geq, in, is };

RelationType relation_type_from_token_type(TokenType tokenType) {
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
  case TokenType::op_in:
    return RelationType::in;
  case TokenType::op_is:
    return RelationType::is;
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
