#ifndef OBERON0C_REPEATSTATEMENTNODE_H
#define OBERON0C_REPEATSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;

class RepeatStatementNode final : public Node {
public:
  RepeatStatementNode(const FilePos &pos)
      : Node(NodeType::repeat_statement, pos) {}
  ~RepeatStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_REPEATSTATEMENTNODE_H
