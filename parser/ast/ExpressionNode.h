#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "Node.h"
#include "RelationNode.h"
#include <memory>

class ExpressionNode final : public Node {
public:
  ExpressionNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~ExpressionNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<SimpleExprNode> left_expr;
  std::unique_ptr<RelationNode> relation;
  std::unique_ptr<SimpleExprNode> right_expr;
};

#endif // OBERON0C_EXPRESSIONNODE_H
