#ifndef OBERON0C_PROCEDUREHEADINGNODE_H
#define OBERON0C_PROCEDUREHEADINGNODE_H

#include "FormalParametersNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class ProcedureHeadingNode final : public Node {
public:
  ProcedureHeadingNode(const FilePos &pos)
      : Node(NodeType::procedure_heading, pos) {}
  ~ProcedureHeadingNode() override = default;

  void accept(NodeVisitor &visitor) override {}
  void print(std::ostream &stream) const override {}

  string ident;
  unique_ptr<FormalParametersNode> formal_parameters;
};

#endif // OBERON0C_PROCEDUREHEADINGNODE_H
