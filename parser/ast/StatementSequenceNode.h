#ifndef OBERON0C_STATEMENTSEQUENCENODE_H
#define OBERON0C_STATEMENTSEQUENCENODE_H

#include "Node.h"
#include "StatementNode.h"
#include <memory>
#include <vector>

class StatementSequenceNode final : public Node {
public:
  StatementSequenceNode(const FilePos &pos)
      : Node(NodeType::statement_sequence, pos) {}
  ~StatementSequenceNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::vector<std::unique_ptr<StatementNode>> statements;
};

#endif // OBERON0C_STATEMENTSEQUENCENODE_H
