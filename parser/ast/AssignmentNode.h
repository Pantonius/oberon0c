#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include "DesignatorNode.h"
#include "ExpressionNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>

class AssignmentNode final : public Node {
public:
  AssignmentNode(const FilePos &pos) : Node(NodeType::assignment, pos) {}
  ~AssignmentNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<DesignatorNode> designator;
  std::unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_ASSIGNMENTNODE_H
