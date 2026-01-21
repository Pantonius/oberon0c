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
  TypeNode *type_;

public:
  ExpressionNode(const NodeType &type, const FilePos &pos, TypeNode *type_node)
      : Node(type, pos), type_(type_node) {};
  ~ExpressionNode() = default;

  [[nodiscard]] virtual bool is_const() const = 0;

  void setType(TypeNode *type);
  [[nodiscard]] virtual TypeNode *getType() const;
};

class SelectorNode : public Node {
public:
  SelectorNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~SelectorNode() override = default;
};

class ArrayIndexNode final : public SelectorNode {
public:
  ArrayIndexNode(const FilePos &pos, unique_ptr<ExpressionNode> expression)
      : SelectorNode(NodeType::array_selector, pos),
        expression(std::move(expression)) {}
  ~ArrayIndexNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<ExpressionNode> expression;
};

class RecordFieldNode final : public SelectorNode {
public:
  RecordFieldNode(const FilePos &pos, unique_ptr<IdentNode> ident)
      : SelectorNode(NodeType::record_selector, pos), ident(std::move(ident)) {}
  ~RecordFieldNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> ident;
};

const set<TokenType> UNARY_OP_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_not};
enum class UnaryOpType { plus, minus, u_not };

const set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                         TokenType::op_minus};

UnaryOpType sign_from_token_type(TokenType tokenType);

class UnaryExpressionNode final : public ExpressionNode {
public:
  UnaryExpressionNode(const FilePos &pos, UnaryOpType &op,
                      unique_ptr<ExpressionNode> expression,
                      TypeNode *type_node)
      : ExpressionNode(NodeType::unary_expression, pos, type_node), op(op),
        expression(std::move(expression)) {}
  ~UnaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final;

  const UnaryOpType &op;
  const unique_ptr<ExpressionNode> expression;
};

// TODO think about merge with IdentNode
class IdentExpressionNode final : public ExpressionNode {
public:
  IdentExpressionNode(const FilePos &pos, unique_ptr<IdentNode> ident,
                      vector<unique_ptr<SelectorNode>> &selectors,
                      TypeNode *type_node)
      : ExpressionNode(NodeType::ident_expression, pos, type_node),
        ident(std::move(ident)), selectors(selectors) {}
  ~IdentExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final;

  const unique_ptr<IdentNode> ident;
  const vector<unique_ptr<SelectorNode>> &selectors;
};

class NumberExpressionNode final : public ExpressionNode {
public:
  NumberExpressionNode(const FilePos &pos, int number, TypeNode *type_node)
      : ExpressionNode(NodeType::number_expression, pos, type_node),
        number(number) {}
  ~NumberExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
  bool is_const() const final { return true; };

  const int number;
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
  BinaryExpressionNode(const FilePos &pos,
                       unique_ptr<ExpressionNode> left_expression,
                       const BinaryOpType &op,
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
  const BinaryOpType &op;
  const unique_ptr<ExpressionNode> right_expression;
};

#endif // OBERON0C_EXPRESSIONNODE_H
