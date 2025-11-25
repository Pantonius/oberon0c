#ifndef OBERON0C_TYPEDECLARATIONNODE_H
#define OBERON0C_TYPEDECLARATIONNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class TypeDeclarationNode final : public Node {
public:
  TypeDeclarationNode(const FilePos &pos)
      : Node(NodeType::type_declaration, pos) {}
  ~TypeDeclarationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  std::unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_TYPEDECLARATIONNODE_H
