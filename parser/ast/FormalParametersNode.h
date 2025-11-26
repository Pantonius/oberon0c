#ifndef OBERON0C_FORMALPARAMETERSNODE_H
#define OBERON0C_FORMALPARAMETERSNODE_H

#include "FPSectionNode.h"
#include "Node.h"
#include "QualIdentNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class FormalParametersNode final : public Node {
public:
  FormalParametersNode(const FilePos &pos)
      : Node(NodeType::formal_parameters, pos) {}
  ~FormalParametersNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<QualIdentNode> qual_ident;
  vector<std::unique_ptr<FPSectionNode>> sections;
};

#endif // OBERON0C_FORMALPARAMETERSNODE_H
