#ifndef OBERON0C_STATEMENTSEQUENCENODE_H
#define OBERON0C_STATEMENTSEQUENCENODE_H

#include "Node.h"
#include "StatementNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class StatementSequenceNode final : public Node {
public:
  StatementSequenceNode(const FilePos &pos)
      : Node(NodeType::statement_sequence, pos) {}
  ~StatementSequenceNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<StatementNode>> statements;
};

#endif // OBERON0C_STATEMENTSEQUENCENODE_H
