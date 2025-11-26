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
  ~ProcedureCallNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  vector<unique_ptr<ExpressionNode>> parameters;
};

#endif // OBERON0C_PROCEDURECALLNODE_H
