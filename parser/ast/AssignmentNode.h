#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include "DesignatorNode.h"
#include "ExpressionNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>
using std::unique_ptr;

class AssignmentNode final : public Node {
public:
  AssignmentNode(const FilePos &pos) : Node(NodeType::assignment, pos) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<DesignatorNode> designator;
  unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_ASSIGNMENTNODE_H
