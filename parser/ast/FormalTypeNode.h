#ifndef OBERON0C_FORMALTYPENODE_H
#define OBERON0C_FORMALTYPENODE_H

#include "Node.h"
#include "QualIdentNode.h"
#include <memory>
#include <vector>

class FormalTypeNode final : public Node {
public:
  FormalTypeNode(const FilePos &pos) : Node(NodeType::formal_type, pos) {}
  ~FormalTypeNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<QualIdentNode> qual_ident;
};

#endif // OBERON0C_FORMALTYPENODE_H
