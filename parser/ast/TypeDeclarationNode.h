#ifndef OBERON0C_TYPEDECLARATIONNODE_H
#define OBERON0C_TYPEDECLARATIONNODE_H

#include "Node.h"
#include "TypeNode.h"
#include <memory>

using std::unique_ptr;

class TypeDeclarationNode final : public Node {
public:
  TypeDeclarationNode(const FilePos &pos)
      : Node(NodeType::type_declaration, pos) {}
  ~TypeDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  // FIXME: Should be a TypeNode
  unique_ptr<TypeNode> type;
};

#endif // OBERON0C_TYPEDECLARATIONNODE_H
