#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "SelectorNode.h"
#include "scanner/Token.h"
#include <memory>
using std::unique_ptr;

class AssignmentNode final : public Node {
public:
  AssignmentNode(const FilePos &pos) : Node(NodeType::assignment, pos) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
  unique_ptr<SelectorNode> selector;
};

#endif // OBERON0C_ASSIGNMENTNODE_H
