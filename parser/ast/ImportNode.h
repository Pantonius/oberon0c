#ifndef OBERON0C_IMPORTNODE_H
#define OBERON0C_IMPORTNODE_H

#include "Node.h"

using std::string;

class ImportNode final : public Node {
public:
  ImportNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~ImportNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string alias_ident;
  string actual_ident;
};

#endif // OBERON0C_IMPORTNODE_H
