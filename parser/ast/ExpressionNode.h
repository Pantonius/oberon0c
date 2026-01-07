#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "IdentNode.h"
#include "Node.h"
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
public:
  ExpressionNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {};
  ~ExpressionNode() override = default;
};

class SelectorNode : public Node {
public:
  SelectorNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~SelectorNode() override = default;
};

class ArrayIndexNode final : public SelectorNode {
public:
  ArrayIndexNode(const FilePos &pos, unique_ptr<ExpressionNode> &expression)
      : SelectorNode(NodeType::array_selector, pos), expression(expression) {}
  ~ArrayIndexNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<ExpressionNode> &expression;
};

class RecordFieldNode final : public SelectorNode {
public:
  RecordFieldNode(const FilePos &pos, unique_ptr<IdentNode> &ident)
      : SelectorNode(NodeType::record_selector, pos), ident(ident) {}
  ~RecordFieldNode() override = default;

  void accept(NodeVisitor &) final;
  void print(std::ostream &) const final;

  const unique_ptr<IdentNode> &ident;
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
                      unique_ptr<ExpressionNode> &expression)
      : ExpressionNode(NodeType::unary_expression, pos), op(op),
        expression(expression) {}
  ~UnaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const UnaryOpType &op;
  const unique_ptr<ExpressionNode> &expression;
};

class IdentExpressionNode final : public ExpressionNode {
public:
  IdentExpressionNode(const FilePos &pos, unique_ptr<IdentNode> &ident,
                      vector<unique_ptr<SelectorNode>> &selectors)
      : ExpressionNode(NodeType::ident_expression, pos), ident(ident),
        selectors(selectors) {}
  ~IdentExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> &ident;
  const vector<unique_ptr<SelectorNode>> &selectors;
};

class NumberExpressionNode final : public ExpressionNode {
public:
  NumberExpressionNode(const FilePos &pos, int &number)
      : ExpressionNode(NodeType::number_expression, pos), number(number) {}
  ~NumberExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const int &number;
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
                       unique_ptr<ExpressionNode> &left_expression,
                       const BinaryOpType &op,
                       unique_ptr<ExpressionNode> &right_expression)
      : ExpressionNode(NodeType::binary_expression, pos),
        left_expression(left_expression), op(op),
        right_expression(right_expression) {}
  ~BinaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> &left_expression;
  const BinaryOpType &op;
  const unique_ptr<ExpressionNode> &right_expression;
};

#endif // OBERON0C_EXPRESSIONNODE_H
