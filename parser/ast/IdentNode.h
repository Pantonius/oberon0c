#ifndef OBERON0C_IDENTNODE_H
#define OBERON0C_IDENTNODE_H

#include "Node.h"
#include <memory>

using std::string;
using std::unique_ptr;

class IdentNode final : public Node {
public:
  IdentNode(const FilePos &pos, string ident)
      : Node(NodeType::ident, pos), value(ident) {}
  ~IdentNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const string value;
};

#endif // OBERON0C_IDENTNODE_H
