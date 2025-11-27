#ifndef OBERON0C_TYPEDECLARATIONNODE_H
#define OBERON0C_TYPEDECLARATIONNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class TypeDeclarationNode final : public Node {
public:
  TypeDeclarationNode(const FilePos &pos)
      : Node(NodeType::type_declaration, pos) {}
  ~TypeDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_TYPEDECLARATIONNODE_H
