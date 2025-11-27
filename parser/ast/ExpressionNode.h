#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "Node.h"
#include "SimpleExprNode.h"
#include "scanner/Token.h"
#include <memory>

using std::unique_ptr;

enum class RelationType { eq, neq, lt, leq, gt, geq, in, is };

const set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq, TokenType::op_neq, TokenType::op_lt, TokenType::op_leq,
    TokenType::op_gt, TokenType::op_geq, TokenType::op_in, TokenType::op_is};

RelationType relation_from_token_type(TokenType tokenType);
class ExpressionNode final : public Node {
public:
  ExpressionNode(const FilePos &pos) : Node(NodeType::expression, pos) {}
  ~ExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<SimpleExprNode> left_expr;
  RelationType relation;
  unique_ptr<SimpleExprNode> right_expr;
};

#endif // OBERON0C_EXPRESSIONNODE_H
