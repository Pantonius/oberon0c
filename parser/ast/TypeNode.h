#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "ArrayTypeNode.h"
#include "Node.h"
#include "RecordTypeNode.h"
#include <memory>

using std::string;
using std::unique_ptr;

class TypeNode final : public Node {
public:
  TypeNode(const FilePos &pos) : Node(NodeType::type, pos) {}
  ~TypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ArrayTypeNode> array_type;
  unique_ptr<RecordTypeNode> record_type;
};

#endif // OBERON0C_TYPENODE_H
