#ifndef OBERON0C_FUNCTIONCALL_H
#define OBERON0C_FUNCTIONCALL_H

#include "ActualParametersNode.h"
#include "DesignatorNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>

class FunctionCallNode final : public Node {
public:
  FunctionCallNode(const FilePos &pos) : Node(NodeType::function_call, pos) {}
  ~FunctionCallNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<DesignatorNode> designator;
  std::unique_ptr<ActualParametersNode> actual_parameters;
};

#endif // OBERON0C_FUNCTIONCALL_H
