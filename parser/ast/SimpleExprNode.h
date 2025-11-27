#ifndef OBERON0C_SIMPLEEXPRNODE_H
#define OBERON0C_SIMPLEEXPRNODE_H

#include "Node.h"
#include "TermNode.h"
#include <memory>
#include <utility>
#include <vector>

using std::pair;
using std::set;
using std::unique_ptr;
using std::vector;

const set<TokenType> SIGN_TOKEN_TYPES = {TokenType::op_plus,
                                         TokenType::op_minus};
enum class SignType { plus, minus };

SignType sign_from_token_type(TokenType tokenType);

const set<TokenType> ADD_OPERATOR_TOKEN_TYPES = {
    TokenType::op_plus, TokenType::op_minus, TokenType::op_or};
enum class AddOperatorType { plus, minus, a_or };

AddOperatorType add_operator_from_token_type(TokenType tokenType);

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

#endif // OBERON0C_SIMPLEEXPRNODE_H
