#ifndef OBERON0C_PROCEDURECALLNODE_H
#define OBERON0C_PROCEDURECALLNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class ProcedureCallNode final : public Node {
public:
  ProcedureCallNode(const FilePos &pos) : Node(NodeType::procedure_call, pos) {}
  ~ProcedureCallNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<SelectorNode> selector_node;
  unique_ptr<ActualParametersNode> actual_parameters;
};

#endif // OBERON0C_PROCEDURECALLNODE_H
