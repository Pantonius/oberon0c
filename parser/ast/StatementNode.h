#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class DeclarationNode;
class StatementSequenceNode;

class StatementNode : public Node {
public:
  StatementNode(const NodeType &type, const FilePos pos) : Node(type, pos) {}
  ~StatementNode() override = default;
};

class AssignmentNode final : public StatementNode {
public:
  explicit AssignmentNode(const FilePos pos,
                          unique_ptr<IdentExpressionNode> ident_expr,
                          unique_ptr<ExpressionNode> expression)
      : StatementNode(NodeType::assignment, pos),
        ident_expr(std::move(ident_expr)), expression(std::move(expression)) {}
  ~AssignmentNode() override = default;

  void accept(NodeVisitor &) override final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentExpressionNode> ident_expr;
  const unique_ptr<ExpressionNode> expression;
};

class ElsIfStatementNode final : public StatementNode {
public:
  ElsIfStatementNode(const FilePos pos, unique_ptr<ExpressionNode> condition,
                     unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::elsif_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~ElsIfStatementNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

class IfStatementNode final : public StatementNode {
public:
  IfStatementNode(const FilePos pos, unique_ptr<ExpressionNode> condition,
                  unique_ptr<StatementSequenceNode> body,
                  vector<unique_ptr<ElsIfStatementNode>> elsifs,
                  unique_ptr<StatementSequenceNode> else_statement_sequence)
      : StatementNode(NodeType::if_statement, pos),
        condition(std::move(condition)), body(std::move(body)),
        elsifs(std::move(elsifs)),
        else_statement_sequence(std::move(else_statement_sequence)) {}
  ~IfStatementNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
  const vector<unique_ptr<ElsIfStatementNode>> elsifs;
  const unique_ptr<StatementSequenceNode> else_statement_sequence;
};

class ProcedureCallNode final : public StatementNode {
public:
  explicit ProcedureCallNode(
      const FilePos pos, unique_ptr<IdentNode> ident,
      vector<unique_ptr<SelectorNode>> selectors,
      vector<unique_ptr<ExpressionNode>> actual_parameters,
      const DeclarationNode *ref)
      : StatementNode(NodeType::procedure_call, pos), ident(std::move(ident)),
        selectors(std::move(selectors)),
        actual_parameters(std::move(actual_parameters)), ref(ref) {}
  ~ProcedureCallNode() override = default;

  void accept(NodeVisitor &) override final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> ident;
  const vector<unique_ptr<SelectorNode>> selectors;
  const vector<unique_ptr<ExpressionNode>> actual_parameters;
  const DeclarationNode *ref;
};

class WhileStatementNode final : public StatementNode {
public:
  WhileStatementNode(const FilePos pos, unique_ptr<ExpressionNode> condition,
                     unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::while_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~WhileStatementNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

class RepeatStatementNode final : public StatementNode {
public:
  RepeatStatementNode(const FilePos pos, unique_ptr<ExpressionNode> condition,
                      unique_ptr<StatementSequenceNode> body)
      : StatementNode(NodeType::repeat_statement, pos),
        condition(std::move(condition)), body(std::move(body)) {}
  ~RepeatStatementNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> condition;
  const unique_ptr<StatementSequenceNode> body;
};

class PatternNode : public Node {
public:
  PatternNode(const NodeType &node_type, const FilePos pos, TypeNode *type)
      : Node(node_type, pos), type(type) {}
  ~PatternNode() override = default;

  TypeNode *type;
};

class IdentPatternNode final : public PatternNode {
public:
  IdentPatternNode(const FilePos pos, unique_ptr<VarDeclarationNode> var,
                   TypeNode *type)
      : PatternNode(NodeType::ident_pattern, pos, type), var(std::move(var)) {}
  ~IdentPatternNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<VarDeclarationNode> var;
};

// class ArrayPatternNode final : public PatternNode {
// public:
//   ArrayPatternNode(const FilePos pos,
//                    vector<unique_ptr<PatternNode>> elem_patterns,
//                    TypeNode *type)
//       : PatternNode(NodeType::variant_pattern, pos, type),
//
//         elem_patterns(std::move(elem_patterns)) {}
//   ~ArrayPatternNode() override = default;
//
//   void accept(NodeVisitor &visitor) override final;
//   void print(std::ostream &stream) const final;
//
//   const vector<unique_ptr<PatternNode>> elem_patterns;
// };

class VariantPatternNode final : public PatternNode {
public:
  VariantPatternNode(const FilePos pos, unique_ptr<IdentNode> sum_ident,
                     unique_ptr<RecordFieldNode> variant,
                     vector<unique_ptr<PatternNode>> param_patterns,
                     TypeNode *type)
      : PatternNode(NodeType::variant_pattern, pos, type),
        sum_ident(std::move(sum_ident)), variant(std::move(variant)),
        param_patterns(std::move(param_patterns)) {}
  ~VariantPatternNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> sum_ident;
  const unique_ptr<RecordFieldNode> variant;
  const vector<unique_ptr<PatternNode>> param_patterns;
};

template <typename T> class LiteralPatternNode : public PatternNode {
public:
  LiteralPatternNode(const FilePos pos, T value, TypeNode *type)
      : PatternNode(NodeType::literal_pattern, pos, type), value(value) {}
  ~LiteralPatternNode() override = default;

  const T value;
};

class NumberPatternNode final : public LiteralPatternNode<int32_t> {
public:
  NumberPatternNode(const FilePos pos, int32_t number);
  ~NumberPatternNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;
};

class BooleanPatternNode final : public LiteralPatternNode<bool> {
public:
  BooleanPatternNode(const FilePos pos, bool boolean);
  ~BooleanPatternNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;
};

enum class KeyType { boolean, number, variant, ident };

class CaseTreeKey {
public:
  CaseTreeKey(const KeyType type) : type(type) {};
  ~CaseTreeKey() = default;

  const KeyType type;
};

class BoolCaseTreeKey final : public CaseTreeKey {
public:
  BoolCaseTreeKey(bool value) : CaseTreeKey(KeyType::boolean), value(value) {};
  ~BoolCaseTreeKey() = default;

  const bool value;
};

class NumberCaseTreeKey final : public CaseTreeKey {
public:
  NumberCaseTreeKey(int32_t value)
      : CaseTreeKey(KeyType::number), value(value) {};
  ~NumberCaseTreeKey() = default;

  const int32_t value;
};

class VariantCaseTreeKey final : public CaseTreeKey {
public:
  VariantCaseTreeKey(string value)
      : CaseTreeKey(KeyType::variant), value(value) {};
  ~VariantCaseTreeKey() = default;

  const string value;
};

class IdentCaseTreeKey final : public CaseTreeKey {
public:
  IdentCaseTreeKey() : CaseTreeKey(KeyType::ident) {};
  ~IdentCaseTreeKey() = default;
};

class CaseTree {
public:
  CaseTree(CaseTreeKey key, vector<u_int> cases, size_t level)
      : key(key), level(level), cases(cases) {};
  ~CaseTree() = default;

  CaseTreeKey key;
  size_t level;
  vector<u_int> cases;
  vector<unique_ptr<CaseTree>> children;
};

class CaseStatementNode final : public StatementNode {
private:
  vector<std::pair<unique_ptr<PatternNode>, unique_ptr<StatementSequenceNode>>>
      cases_;

public:
  CaseStatementNode(const FilePos pos, unique_ptr<ExpressionNode> value)
      : StatementNode(NodeType::case_statement, pos), value(std::move(value)) {}
  ~CaseStatementNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> value;

  vector<u_int> reachable_cases;

  void add_case(unique_ptr<PatternNode>, unique_ptr<StatementSequenceNode>);
  vector<
      std::pair<unique_ptr<PatternNode>, unique_ptr<StatementSequenceNode>>> *
  get_cases();
};

#endif // OBERON0C_STATEMENTNODE_H
