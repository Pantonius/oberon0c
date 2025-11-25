#ifndef OBERON0C_TERMNODE_H
#define OBERON0C_TERMNODE_H

#include "FactorNode.h"
#include "Node.h"
#include "scanner/Token.h"
#include <memory>
#include <set>
#include <utility>
#include <vector>

const std::set<TokenType> MUL_OPERATOR_TOKEN_TYPES = {
    TokenType::op_times, TokenType::op_div, TokenType::op_divide,
    TokenType::op_mod, TokenType::op_and};
enum MulOperatorType { m_times, m_div, m_divide, m_mod, m_and };

MulOperatorType mul_operator_from_token_type(TokenType tokenType) {
  switch (tokenType) {
  case TokenType::op_times:
    return m_times;
  case TokenType::op_div:
    return m_div;
  case TokenType::op_divide:
    return m_divide;
  case TokenType::op_mod:
    return m_mod;
  case TokenType::op_and:
    return m_and;
  default:
    exit(EXIT_FAILURE); // TODO review
  }
}

class TermNode final : public Node {
public:
  TermNode(const FilePos &pos) : Node(NodeType::term, pos) {}
  ~TermNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<FactorNode> factor;
  std::vector<std::pair<MulOperatorType, std::unique_ptr<FactorNode>>>
      additional_terms;
};

#endif // OBERON0C_TERMNODE_H
