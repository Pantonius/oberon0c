#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "ActualParametersNode.h"
#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;

class AssignmentNode final : public Node {
public:
  explicit AssignmentNode(const FilePos &pos)
      : Node(NodeType::assignment, pos) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &) override {};
  void print(std::ostream &) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
  unique_ptr<SelectorNode> selector;
};

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

class ProcedureCallNode final : public Node {
public:
  explicit ProcedureCallNode(const FilePos &pos)
      : Node(NodeType::procedure_call, pos) {}
  ~ProcedureCallNode() override = default;

  void accept(NodeVisitor &) override {};
  void print(std::ostream &) const override {};

  string ident;
  unique_ptr<SelectorNode> selector;
  unique_ptr<ActualParametersNode> actual_parameters;
};

class WhileStatementNode final : public Node {
public:
  WhileStatementNode(const FilePos &pos)
      : Node(NodeType::while_statement, pos) {}
  ~WhileStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
};

class RepeatStatementNode final : public Node {
public:
  RepeatStatementNode(const FilePos &pos)
      : Node(NodeType::repeat_statement, pos) {}
  ~RepeatStatementNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> condition;
  unique_ptr<StatementSequenceNode> body;
};

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
  unique_ptr<RepeatStatementNode> repeat_statement;
};

#endif // OBERON0C_STATEMENTNODE_H
