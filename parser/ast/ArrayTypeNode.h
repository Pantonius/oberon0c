#ifndef OBERON0C_ARRAYTYPENODE_H
#define OBERON0C_ARRAYTYPENODE_H

#include "ExpressionNode.h"
#include "Node.h"

class TypeNode;
class ArrayTypeNode final : public Node {
public:
  ArrayTypeNode(const FilePos &pos) : Node(NodeType::array_type, pos) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> expression;
  unique_ptr<TypeNode> type;
};

#endif // OBERON0C_ARRAYTYPENODE_H
