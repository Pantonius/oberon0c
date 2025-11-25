#ifndef OBERON0C_PROCEDUREHEADINGNODE_H
#define OBERON0C_PROCEDUREHEADINGNODE_H

#include "FormalParametersNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class ProcedureHeadingNode final : public Node {
public:
  ProcedureHeadingNode(const FilePos &pos)
      : Node(NodeType::procedure_heading, pos) {}
  ~ProcedureHeadingNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  bool exported;
  std::unique_ptr<FormalParametersNode> formal_parameters;
};

#endif // OBERON0C_PROCEDUREHEADINGNODE_H
