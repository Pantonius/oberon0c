#ifndef OBERON0C_IFSTATEMENTNODE_H
#define OBERON0C_IFSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class IfStatementNode final : public Node {
public:
  IfStatementNode(const FilePos &pos) : Node(NodeType::if_statement, pos) {}
  ~IfStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementSequenceNode> body;
  std::vector<std::pair<std::unique_ptr<ExpressionNode>,
                        std::unique_ptr<StatementSequenceNode>>>
      elsifs;
  std::unique_ptr<StatementSequenceNode> else_statement_sequence;
};

#endif // OBERON0C_IFSTATEMENTNODE_H
