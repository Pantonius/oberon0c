#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

enum class RelationType { eq, neq, lt, leq, gt, geq };

const set<TokenType> RELATION_TOKEN_TYPES = {
    TokenType::op_eq,  TokenType::op_neq, TokenType::op_lt,
    TokenType::op_leq, TokenType::op_gt,  TokenType::op_geq,
};

RelationType relation_from_token_type(TokenType tokenType);

class SimpleExprNode;

class ExpressionNode final : public Node {
public:
  ExpressionNode(const FilePos &pos) : Node(NodeType::expression, pos) {}
  ~ExpressionNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<SimpleExprNode> left_expr;
  RelationType relation;
  unique_ptr<SimpleExprNode> right_expr;
};

class SelectorNode final : public Node {
public:
  explicit SelectorNode(const FilePos &pos) : Node(NodeType::selector, pos) {}
  ~SelectorNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
};

class FactorNode final : public Node {
public:
  explicit FactorNode(const FilePos &pos) : Node(NodeType::factor, pos) {}
  ~FactorNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<SelectorNode> selector;
  int number;
  unique_ptr<ExpressionNode> expression;
  unique_ptr<FactorNode> not_factor;
};

const set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                         TokenType::op_minus};
enum class SignType { plus, minus };

SignType sign_from_token_type(TokenType tokenType);

const set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};
enum class AddOperatorType { plus, minus, a_or };

AddOperatorType add_operator_from_token_type(TokenType tokenType);

const set<TokenType> MUL_OPERATOR_TOKEN_TYPES = {
    TokenType::op_times, TokenType::op_div, TokenType::op_divide,
    TokenType::op_mod, TokenType::op_and};
enum class MulOperatorType { times, div, divide, mod, m_and };

MulOperatorType mul_operator_from_token_type(TokenType tokenType);
class TermNode final : public Node {
public:
  TermNode(const FilePos &pos) : Node(NodeType::term, pos) {}
  ~TermNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<FactorNode> factor;
  vector<pair<MulOperatorType, unique_ptr<FactorNode>>> additional_terms;
};

class SimpleExprNode final : public Node {
public:
  SimpleExprNode(const FilePos &pos) : Node(NodeType::simple_expr, pos) {}
  ~SimpleExprNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  SignType sign;
  unique_ptr<TermNode> term;
  vector<pair<AddOperatorType, unique_ptr<TermNode>>> additional_terms;
};

#endif // OBERON0C_EXPRESSIONNODE_H
