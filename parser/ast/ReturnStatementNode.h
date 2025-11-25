#ifndef OBERON0C_RETURNSTATEMENTNODE_H
#define OBERON0C_RETURNSTATEMENTNODE_H

#include "Node.h"
#include <memory>
#include <vector>

class ReturnStatementNode final : public Node {
public:
  ReturnStatementNode(const FilePos &pos)
      : Node(NodeType::return_statement, pos) {}
  ~ReturnStatementNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_RETURNSTATEMENTNODE_H
