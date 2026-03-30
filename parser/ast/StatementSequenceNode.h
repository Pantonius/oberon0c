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
  StatementSequenceNode(const FilePos pos,
                        vector<unique_ptr<StatementNode>> stmts)
      : Node(NodeType::statement_sequence, pos), stmts(std::move(stmts)) {}
  ~StatementSequenceNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  vector<unique_ptr<StatementNode>> stmts;
};

#endif // OBERON0C_STATEMENTSEQUENCENODE_H
