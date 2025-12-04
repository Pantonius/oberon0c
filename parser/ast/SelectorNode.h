#ifndef OBERON0C_SELECTORNODE_H
#define OBERON0C_SELECTORNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::string;
using std::unique_ptr;

class SelectorNode final : public Node {
public:
  SelectorNode(const FilePos &pos) : Node(NodeType::selector, pos) {}
  ~SelectorNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
};

#endif // !OBERON0C_SELECTORNODE_H
