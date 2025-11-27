#ifndef OBERON0C_FACTORNODE_H
#define OBERON0C_FACTORNODE_H

#include "Node.h"
#include <memory>

using std::unique_ptr;

class ExpressionNode;

class FactorNode final : public Node {
public:
  FactorNode(const FilePos &pos) : Node(NodeType::factor, pos) {}
  ~FactorNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  bool nil = false;
  unique_ptr<ExpressionNode> expression;
  unique_ptr<FactorNode> not_factor;
};

#endif // OBERON0C_FACTORNODE_H
