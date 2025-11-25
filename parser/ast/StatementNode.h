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

class StatementNode final : public Node {
public:
  StatementNode(const FilePos &pos) : Node(NodeType::statement, pos) {}
  ~StatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<IfStatementNode> if_statement;
  std::unique_ptr<CaseStatementNode> case_statement;
  std::unique_ptr<LoopStatementNode> loop_statement;
  std::unique_ptr<WhileStatementNode> while_statement;
  std::unique_ptr<WithStatementNode> with_statement;
  std::unique_ptr<ExitStatementNode> exit_statement;
  std::unique_ptr<ReturnStatementNode> return_statement;
};

#endif // OBERON0C_STATEMENTNODE_H
