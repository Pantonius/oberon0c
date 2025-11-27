#ifndef OBERON0C_CLAUSENODE_H
#define OBERON0C_CLAUSENODE_H

#include "CaseLabelListNode.h"
#include "Node.h"
#include <memory>

using std::unique_ptr;

class StatementSequenceNode;

class ClauseNode final : public Node {
public:
  ClauseNode(const FilePos &pos) : Node(NodeType::clause, pos) {}
  ~ClauseNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<CaseLabelListNode> case_label_list;
  unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_CLAUSENODE_H
