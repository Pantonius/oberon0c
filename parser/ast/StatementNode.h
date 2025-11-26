#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "CaseStatementNode.h"
#include "ExitStatementNode.h"
#include "IfStatementNode.h"
#include "LoopStatementNode.h"
#include "Node.h"
#include "ReturnStatementNode.h"
#include "WhileStatementNode.h"
#include "WithStatementNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;

class StatementNode final : public Node {
public:
  StatementNode(const FilePos &pos) : Node(NodeType::statement, pos) {}
  ~StatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<IfStatementNode> if_statement;
  unique_ptr<CaseStatementNode> case_statement;
  unique_ptr<LoopStatementNode> loop_statement;
  unique_ptr<WhileStatementNode> while_statement;
  unique_ptr<WithStatementNode> with_statement;
  unique_ptr<ExitStatementNode> exit_statement;
  unique_ptr<ReturnStatementNode> return_statement;
};

#endif // OBERON0C_STATEMENTNODE_H
