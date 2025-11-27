#ifndef OBERON0C_LOOPSTATEMENTNODE_H
#define OBERON0C_LOOPSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;

class LoopStatementNode final : public Node {
public:
  LoopStatementNode(const FilePos &pos) : Node(NodeType::loop_statement, pos) {}
  ~LoopStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_LOOPSTATEMENTNODE_H
