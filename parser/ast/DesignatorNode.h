#ifndef OBERON0C_DESIGNATORNODE_H
#define OBERON0C_DESIGNATORNODE_H

#include "Node.h"

class DesignatorNode final : public Node {
public:
  DesignatorNode(const FilePos &pos) : Node(NodeType::designator, pos) {}
  ~DesignatorNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
};

#endif // OBERON0C_DESIGNATORNODE_H
