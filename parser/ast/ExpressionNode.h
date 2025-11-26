#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "Node.h"
#include "RelationNode.h"
#include "SimpleExprNode.h"
#include <memory>

using std::unique_ptr;

class ExpressionNode final : public Node {
public:
  ExpressionNode(const FilePos &pos) : Node(NodeType::expression, pos) {}
  ~ExpressionNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<SimpleExprNode> left_expr;
  RelationType relation;
  unique_ptr<SimpleExprNode> right_expr;
};

#endif // OBERON0C_EXPRESSIONNODE_H
