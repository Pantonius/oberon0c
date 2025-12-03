#ifndef OBERON0C_FPSECTIONNODE_H
#define OBERON0C_FPSECTIONNODE_H

#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class FPSectionNode final : public Node {
public:
  FPSectionNode(const FilePos &pos) : Node(NodeType::fp_section, pos) {}
  ~FPSectionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  vector<string> idents;
  unique_ptr<TypeNode> type;
};

#endif // OBERON0C_FPSECTIONNODE_H
