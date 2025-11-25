#ifndef OBERON0C_VARDECLARATIONNODE_H
#define OBERON0C_VARDECLARATIONNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class VarDeclarationNode final : public Node {
public:
  VarDeclarationNode(const FilePos &pos)
      : Node(NodeType::var_declaration, pos) {}
  ~VarDeclarationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  string ident;
  bool exported;
  std::unique_ptr<ExpressionNode> expression;
  string type;
};

#endif // OBERON0C_VARDECLARATIONNODE_H
