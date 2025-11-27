#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "AssignmentNode.h"
#include "IfStatementNode.h"
#include "Node.h"
#include "ProcedureCallNode.h"
#include "WhileStatementNode.h"
#include <memory>

using std::unique_ptr;

class StatementNode final : public Node {
public:
  StatementNode(const FilePos &pos) : Node(NodeType::statement, pos) {}
  ~StatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<AssignmentNode> assignment;
  unique_ptr<ProcedureCallNode> procedure_call;
  unique_ptr<IfStatementNode> if_statement;
  unique_ptr<WhileStatementNode> while_statement;
};

#endif // OBERON0C_STATEMENTNODE_H
