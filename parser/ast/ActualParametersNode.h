#ifndef OBERON0C_ACTUALPARAMETERSNODE_H
#define OBERON0C_ACTUALPARAMETERSNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class ActualParametersNode final : public Node {
public:
  ActualParametersNode(const FilePos &pos)
      : Node(NodeType::actual_parameters, pos) {}
  ~ActualParametersNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<ExpressionNode>> expressions;
};

#endif // OBERON0C_ACTUALPARAMETERSNODE_H
