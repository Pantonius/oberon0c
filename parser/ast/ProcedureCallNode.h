#ifndef OBERON0C_PROCEDURECALLNODE_H
#define OBERON0C_PROCEDURECALLNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class ProcedureCallNode final : public Node {
public:
  ProcedureCallNode(const FilePos &pos) : Node(NodeType::procedure_call, pos) {}
  ~ProcedureCallNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::string ident;
  std::vector<std::shared_ptr<ExpressionNode>> parameters;
};

#endif // OBERON0C_PROCEDURECALLNODE_H
