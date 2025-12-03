#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "Node.h"
#include <memory>

using std::unique_ptr;

class TypeNode final : public Node {
public:
  TypeNode(const FilePos &pos) : Node(NodeType::type, pos) {}
  ~TypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};
};

#endif // OBERON0C_TYPENODE_H
