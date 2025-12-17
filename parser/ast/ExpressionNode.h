#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

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

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};
};

enum class RelationType { eq, neq, lt, leq, gt, geq, in, is };

const set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq, TokenType::op_neq, TokenType::op_lt, TokenType::op_leq,
    TokenType::op_gt, TokenType::op_geq, TokenType::op_in, TokenType::op_is};

RelationType relation_from_token_type(TokenType tokenType);

const set<TokenType> UNARY_OP_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_not};
enum class UnaryOpType { plus, minus, not_ };

const set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                         TokenType::op_minus};

UnaryOpType sign_from_token_type(TokenType tokenType);

class SelectorNode final : public Node {
public:
  explicit SelectorNode(const FilePos &pos) : Node(NodeType::selector, pos) {}
  ~SelectorNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
};

class UnaryExpressionNode final : public ExpressionNode {
public:
  UnaryExpressionNode(const FilePos &pos)
      : ExpressionNode(NodeType::unary_expression, pos) {}
  ~UnaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  UnaryOpType op;
  unique_ptr<ExpressionNode> expression;
};

class IdentExpressionNode final : public ExpressionNode {
public:
  IdentExpressionNode(const FilePos &pos)
      : ExpressionNode(NodeType::ident_expression, pos) {}
  ~IdentExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<SelectorNode> selector;
};

class NumberExpressionNode final : public ExpressionNode {
public:
  NumberExpressionNode(const FilePos &pos)
      : ExpressionNode(NodeType::number_expression, pos) {}
  ~NumberExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  int number;
};

const set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};
enum class AddOperatorType { plus, minus, a_or };

AddOperatorType add_operator_from_token_type(TokenType tokenType);

const set<TokenType> MUL_OPERATOR_TOKEN_TYPES = {
    TokenType::op_times, TokenType::op_div, TokenType::op_divide,
    TokenType::op_mod, TokenType::op_and};
enum class MulOperatorType { times, div, divide, mod, m_and };

MulOperatorType mul_operator_from_token_type(TokenType tokenType);

class BinaryExpressionNode final : public ExpressionNode {
public:
  BinaryExpressionNode(const FilePos &pos)
      : ExpressionNode(NodeType::binary_expression, pos) {}
  ~BinaryExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> left_expression;
  variant<AddOperatorType, MulOperatorType, RelationType> op;
  unique_ptr<ExpressionNode> right_expression;
};

#endif // OBERON0C_EXPRESSIONNODE_H
