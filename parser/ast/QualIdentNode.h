#ifndef OBERON0C_QUALIDENTNODE_H
#define OBERON0C_QUALIDENTNODE_H

#include "Node.h"

class QualIdentNode final : public Node {
public:
  QualIdentNode(const FilePos &pos) : Node(NodeType::qual_ident, pos) {}
  ~QualIdentNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string module;
  string ident;
};

#endif // OBERON0C_QUALIDENTNODE_H
