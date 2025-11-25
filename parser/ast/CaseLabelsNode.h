#ifndef OBERON0C_CASELABELSNODE_H
#define OBERON0C_CASELABELSNODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class CaseLabelsNode final : public Node {
public:
  CaseLabelsNode(const FilePos &pos) : Node(NodeType::case_labels, pos) {}
  ~CaseLabelsNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<ExpressionNode> const_expression;
  std::unique_ptr<ExpressionNode> until_const_expression;
};

#endif // OBERON0C_CASELABELsNODE_H
