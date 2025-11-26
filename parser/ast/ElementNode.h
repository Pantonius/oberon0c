#ifndef OBERON0C_ELEMENTNODE_H
#define OBERON0C_ELEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>

using std::unique_ptr;

class ElementNode final : public Node {
public:
  ElementNode(const FilePos &pos) : Node(NodeType::element, pos) {}
  ~ElementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<ExpressionNode> expression;
  unique_ptr<ExpressionNode> until_expression;
};

#endif // OBERON0C_ELEMENTNODE_H
