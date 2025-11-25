#ifndef OBERON0C_ACTUALPARAMETERSNODE_H
#define OBERON0C_ACTUALPARAMETERSNODE_H

#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <vector>

class ActualParametersNode final : public Node {
public:
  ActualParametersNode(const FilePos &pos)
      : Node(NodeType::actual_parameters, pos) {}
  ~ActualParametersNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::vector<std::unique_ptr<ExpressionNode>> expressions;
};

#endif // OBERON0C_ACTUALPARAMETERSNODE_H
