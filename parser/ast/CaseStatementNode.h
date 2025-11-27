#ifndef OBERON0C_CASESTATEMENTNODE_H
#define OBERON0C_CASESTATEMENTNODE_H

#include "ClauseNode.h"
#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class StatementSequenceNode;

class CaseStatementNode final : public Node {
public:
  CaseStatementNode(const FilePos &pos) : Node(NodeType::case_statement, pos) {}
  ~CaseStatementNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<ExpressionNode> expression;
  vector<unique_ptr<ClauseNode>> clauses;
  unique_ptr<StatementSequenceNode> else_statement_sequence;
};

#endif // OBERON0C_CASESTATEMENTNODE_H
