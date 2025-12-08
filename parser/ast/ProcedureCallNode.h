#ifndef OBERON0C_PROCEDURECALLNODE_H
#define OBERON0C_PROCEDURECALLNODE_H

#include "Node.h"
#include "ActualParametersNode.h"
#include "ExpressionNode.h"
#include <memory>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class ProcedureCallNode final : public Node {
public:
  explicit ProcedureCallNode(const FilePos &pos) : Node(NodeType::procedure_call, pos) {}
  ~ProcedureCallNode() override = default;

  void accept(NodeVisitor &) override {};
  void print(std::ostream &) const override {};

  string ident;
  unique_ptr<SelectorNode> selector;
  unique_ptr<ActualParametersNode> actual_parameters;
};

#endif // OBERON0C_PROCEDURECALLNODE_H
