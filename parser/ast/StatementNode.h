#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;
class StatementNode : public Node {
public:
  StatementNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~StatementNode() override = default;
};

class AssignmentNode final : public StatementNode {
public:
  explicit AssignmentNode(const FilePos &pos, unique_ptr<IdentNode> ident,
                          vector<unique_ptr<SelectorNode>> &selectors,
                          unique_ptr<ExpressionNode> expression)
      : StatementNode(NodeType::assignment, pos), ident(std::move(ident)),
        selectors(selectors), expression(std::move(expression)) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> ident;
  const vector<unique_ptr<SelectorNode>> &selectors;
  const unique_ptr<ExpressionNode> expression;
};

class ElsIfStatementNode final : public StatementNode {
public:
  ElsIfStatementNode(const FilePos &pos, unique_ptr<ExpressionNode> condition,
                     unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::elsif_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~ElsIfStatementNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

class IfStatementNode final : public StatementNode {
public:
  IfStatementNode(const FilePos &pos, unique_ptr<ExpressionNode> condition,
                  unique_ptr<StatementSequenceNode> body,
                  vector<unique_ptr<ElsIfStatementNode>> &elsifs,
                  unique_ptr<StatementSequenceNode> else_statement_sequence)
      : StatementNode(NodeType::if_statement, pos),
        condition(std::move(condition)), body(std::move(body)), elsifs(elsifs),
        else_statement_sequence(std::move(else_statement_sequence)) {}
  ~IfStatementNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
  const vector<unique_ptr<ElsIfStatementNode>> &elsifs;
  const unique_ptr<StatementSequenceNode> else_statement_sequence;
};

class ProcedureCallNode final : public StatementNode {
public:
  explicit ProcedureCallNode(
      const FilePos &pos, unique_ptr<IdentNode> ident,
      vector<unique_ptr<SelectorNode>> &selectors,
      vector<unique_ptr<ExpressionNode>> &actual_parameters)
      : StatementNode(NodeType::procedure_call, pos), ident(std::move(ident)),
        selectors(selectors), actual_parameters(actual_parameters) {}
  ~ProcedureCallNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> ident;
  const vector<unique_ptr<SelectorNode>> &selectors;
  const vector<unique_ptr<ExpressionNode>> &actual_parameters;
};

class WhileStatementNode final : public StatementNode {
public:
  WhileStatementNode(const FilePos &pos, unique_ptr<ExpressionNode> condition,
                     unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::while_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~WhileStatementNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

class RepeatStatementNode final : public StatementNode {
public:
  RepeatStatementNode(const FilePos &pos, unique_ptr<ExpressionNode> condition,
                      unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::repeat_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~RepeatStatementNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

#endif // OBERON0C_STATEMENTNODE_H
