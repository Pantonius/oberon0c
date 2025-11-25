#ifndef OBERON0C_WITHSTATEMENTNODE_H
#define OBERON0C_WITHSTATEMENTNODE_H

#include "Node.h"
#include "QualIdentNode.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class WithStatementNode final : public Node {
public:
  WithStatementNode(const FilePos &pos) : Node(NodeType::with_statement, pos) {}
  ~WithStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<QualIdentNode> first_qual_ident;
  std::unique_ptr<QualIdentNode> second_qual_ident;
  std::unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_WITHSTATEMENTNODE_H
