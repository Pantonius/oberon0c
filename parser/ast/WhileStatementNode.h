#ifndef OBERON0C_WHILESTATEMENTNODE_H
#define OBERON0C_WHILESTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;

class WhileStatementNode final : public Node {
public:
  WhileStatementNode(const FilePos &pos)
      : Node(NodeType::while_statement, pos) {}
  ~WhileStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_WHILESTATEMENTNODE_H
