#ifndef OBERON0C_FACTORNODE_H
#define OBERON0C_FACTORNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <set>
#include <utility>
#include <vector>

class FactorNode final : public Node {
public:
  FactorNode(const FilePos &pos) : Node(NodeType::factor, pos) {}
  ~FactorNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  bool nil = false;
  std::unique_ptr<ExpressionNode> expression;
  std::unique_ptr<FactorNode> not_factor;
};

#endif // OBERON0C_FACTORNODE_H
