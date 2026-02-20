#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "Node.h"
#include "TypeNode.h"
#include "scanner/Token.h"
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

using std::pair;
using std::set;
using std::string;
using std::unique_ptr;
using std::variant;
using std::vector;

class ExpressionNode : public Node {
private:
public:
  ExpressionNode(const NodeType &type, const FilePos pos, TypeNode *type_node)
      : Node(type, pos), type(type_node) {};
  ~ExpressionNode() = default;

  TypeNode *type;

  [[nodiscard]] virtual bool is_const() const = 0;
};

class SelectorNode : public Node {
public:
  SelectorNode(const NodeType &type, const FilePos pos) : Node(type, pos) {}
  ~SelectorNode() override = default;
};

class ArrayIndexNode final : public SelectorNode {
public:
  ArrayIndexNode(const FilePos pos, unique_ptr<ExpressionNode> expression)
      : SelectorNode(NodeType::array_selector, pos),
        expression(std::move(expression)) {}
  ~ArrayIndexNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<ExpressionNode> expression;
};

class RecordFieldNode final : public SelectorNode {
public:
  RecordFieldNode(const FilePos pos, unique_ptr<IdentNode> ident)
      : SelectorNode(NodeType::record_selector, pos), ident(std::move(ident)) {}
  ~RecordFieldNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> ident;
};

const set<TokenType> UNARY_OP_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_not};

enum class UnaryOpType { plus, minus, u_not };

std::ostream &operator<<(std::ostream &, const UnaryOpType &);

const set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                         TokenType::op_minus};

UnaryOpType sign_from_token_type(TokenType tokenType);

class UnaryExpressionNode final : public ExpressionNode {
public:
  UnaryExpressionNode(const FilePos pos, const UnaryOpType op,
                      unique_ptr<ExpressionNode> expression,
                      TypeNode *type_node)
      : ExpressionNode(NodeType::unary_expression, pos, type_node), op(op),
        expression(std::move(expression)) {}
  ~UnaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final;

  const UnaryOpType op;
  const unique_ptr<ExpressionNode> expression;
};

// TODO: think about merge with IdentNode
class IdentExpressionNode final : public ExpressionNode {
public:
  IdentExpressionNode(const FilePos pos, unique_ptr<IdentNode> ident,
                      vector<unique_ptr<SelectorNode>> selectors,
                      TypeNode *type_node)
      : ExpressionNode(NodeType::ident_expression, pos, type_node),
        ident(std::move(ident)), selectors(std::move(selectors)) {}
  ~IdentExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final;

  const unique_ptr<IdentNode> ident;
  const vector<unique_ptr<SelectorNode>> selectors;
};

template <typename T> class LiteralExpressionNode : public ExpressionNode {
public:
  LiteralExpressionNode(const NodeType &type, const FilePos pos, T value,
                        TypeNode *type_node)
      : ExpressionNode(type, pos, type_node), value(value) {};
  ~LiteralExpressionNode() = default;

  const T value;
};

class NumberExpressionNode final : public LiteralExpressionNode<int32_t> {
public:
  NumberExpressionNode(const FilePos pos, int number, TypeNode *type_node)
      : LiteralExpressionNode(NodeType::number, pos, number, type_node) {}
  ~NumberExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final { return true; };
};

class BooleanExpressionNode final : public LiteralExpressionNode<bool> {
public:
  BooleanExpressionNode(const FilePos pos, bool boolean, TypeNode *type_node)
      : LiteralExpressionNode(NodeType::boolean, pos, boolean, type_node) {};
  ~BooleanExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final { return true; };
};

const set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq,  TokenType::op_neq, TokenType::op_lt,
    TokenType::op_leq, TokenType::op_gt,  TokenType::op_geq};

enum class BinaryOpType {
  plus,
  minus,
  b_or,
  times,
  div,
  divide,
  mod,
  b_and,
  eq,
  neq,
  lt,
  leq,
  gt,
  geq
};
std::ostream &operator<<(std::ostream &, const BinaryOpType &);

BinaryOpType relation_from_token_type(TokenType tokenType);
const set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};

BinaryOpType add_operator_from_token_type(TokenType tokenType);

const set<TokenType> MUL_OPERATOR_TOKEN_TYPES = {
    TokenType::op_times, TokenType::op_div, TokenType::op_divide,
    TokenType::op_mod, TokenType::op_and};

BinaryOpType mul_operator_from_token_type(TokenType tokenType);

class BinaryExpressionNode final : public ExpressionNode {
public:
  BinaryExpressionNode(const FilePos pos,
                       unique_ptr<ExpressionNode> left_expression,
                       const BinaryOpType op,
                       unique_ptr<ExpressionNode> right_expression,
                       TypeNode *type_node)
      : ExpressionNode(NodeType::binary_expression, pos, type_node),
        left_expression(std::move(left_expression)), op(op),
        right_expression(std::move(right_expression)) {}
  ~BinaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final;

  const unique_ptr<ExpressionNode> left_expression;
  const BinaryOpType op;
  const unique_ptr<ExpressionNode> right_expression;
};

#endif // OBERON0C_EXPRESSIONNODE_H
