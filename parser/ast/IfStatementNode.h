#ifndef OBERON0C_IFSTATEMENTNODE_H
#define OBERON0C_IFSTATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::pair;
using std::unique_ptr;
using std::vector;

class StatementSequenceNode;

class IfStatementNode final : public Node {
public:
  IfStatementNode(const FilePos &pos) : Node(NodeType::if_statement, pos) {}
  ~IfStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
  vector<pair<unique_ptr<ExpressionNode>, unique_ptr<StatementSequenceNode>>>
      elsifs;
  unique_ptr<StatementSequenceNode> else_statement_sequence;
};

#endif // OBERON0C_IFSTATEMENTNODE_H
