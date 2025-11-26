#ifndef OBERON0C_WHILESTATEMENTNODE_H
#define OBERON0C_WHILESTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>

using std::unique_ptr;

class WhileStatementNode final : public Node {
public:
  WhileStatementNode(const FilePos &pos)
      : Node(NodeType::while_statement, pos) {}
  ~WhileStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_WHILESTATEMENTNODE_H
