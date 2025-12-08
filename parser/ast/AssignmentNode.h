#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include <memory>
#include "ExpressionNode.h"
#include "Node.h"

using std::unique_ptr;

class AssignmentNode final : public Node {
public:
  explicit AssignmentNode(const FilePos &pos) : Node(NodeType::assignment, pos) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &) override {};
  void print(std::ostream &) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
  unique_ptr<SelectorNode> selector;
};

#endif // OBERON0C_ASSIGNMENTNODE_H
