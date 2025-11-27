#ifndef OBERON0C_WITHSTATEMENTNODE_H
#define OBERON0C_WITHSTATEMENTNODE_H

#include "Node.h"
#include "QualIdentNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;

class StatementSequenceNode;

class WithStatementNode final : public Node {
public:
  WithStatementNode(const FilePos &pos) : Node(NodeType::with_statement, pos) {}
  ~WithStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<QualIdentNode> first_qual_ident;
  unique_ptr<QualIdentNode> second_qual_ident;
  unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_WITHSTATEMENTNODE_H
