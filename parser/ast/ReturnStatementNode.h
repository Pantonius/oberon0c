#ifndef OBERON0C_RETURNSTATEMENTNODE_H
#define OBERON0C_RETURNSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class ReturnStatementNode final : public Node {
public:
  ReturnStatementNode(const FilePos &pos)
      : Node(NodeType::return_statement, pos) {}
  ~ReturnStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<ExpressionNode> expression;
};

#endif // OBERON0C_RETURNSTATEMENTNODE_H
