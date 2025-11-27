#ifndef OBERON0C_TERMNODE_H
#define OBERON0C_TERMNODE_H

#include "FactorNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <set>
#include <utility>
#include <vector>

using std::pair;
using std::set;
using std::unique_ptr;
using std::vector;

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

#endif // OBERON0C_TERMNODE_H
