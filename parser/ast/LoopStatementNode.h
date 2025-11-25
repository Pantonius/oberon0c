#ifndef OBERON0C_LOOPSTATEMENTNODE_H
#define OBERON0C_LOOPSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class LoopStatementNode final : public Node {
public:
  LoopStatementNode(const FilePos &pos) : Node(NodeType::loop_statement, pos) {}
  ~LoopStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_LOOPSTATEMENTNODE_H
