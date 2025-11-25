#ifndef OBERON0C_CASESTATEMENTNODE_H
#define OBERON0C_CASESTATEMENTNODE_H

#include "ClauseNode.h"
#include "ExpressionNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class CaseStatementNode final : public Node {
public:
  CaseStatementNode(const FilePos &pos) : Node(NodeType::case_statement, pos) {}
  ~CaseStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<ExpressionNode> expression;
  std::vector<std::unique_ptr<ClauseNode>> clauses;
  std::unique_ptr<StatementSequenceNode> else_statement_sequence;
};

#endif // OBERON0C_CASESTATEMENTNODE_H
