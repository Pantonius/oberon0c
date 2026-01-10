#ifndef OBERON0C_FPSECTIONNODE_H
#define OBERON0C_FPSECTIONNODE_H

#include "IdentNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class TypeNode;

class FPSectionNode final : public Node {
public:
  FPSectionNode(const FilePos &pos, vector<unique_ptr<IdentNode>> &idents,
                unique_ptr<TypeNode> type)
      : Node(NodeType::fp_section, pos), idents(idents), type(std::move(type)) {
  }
  ~FPSectionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const vector<unique_ptr<IdentNode>> &idents;
  const unique_ptr<TypeNode> type;
};

#endif // OBERON0C_FPSECTIONNODE_H
