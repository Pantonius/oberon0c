#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H

#include "Node.h"
#include <memory>
#include <vector>

class StatementNode final : public Node {
public:
  StatementNode(const FilePos &pos) : Node(NodeType::statement, pos) {}
  ~StatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_STATEMENTNODE_H
