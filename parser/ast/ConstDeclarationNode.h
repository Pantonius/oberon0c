#ifndef OBERON0C_CONSTDECLARATIONNODE_H
#define OBERON0C_CONSTDECLARATIONNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class ConstDeclarationNode final : public Node {
public:
  ConstDeclarationNode(const FilePos &pos)
      : Node(NodeType::const_declaration, pos) {}
  ~ConstDeclarationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  bool exported;
  std::unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_CONSTDECLARATIONNODE_H
