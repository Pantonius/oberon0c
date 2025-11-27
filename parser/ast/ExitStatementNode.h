#ifndef OBERON0C_EXITSTATEMENTNODE_H
#define OBERON0C_EXITSTATEMENTNODE_H

#include "Node.h"
#include <memory>
#include <vector>

class ExitStatementNode final : public Node {
public:
  ExitStatementNode(const FilePos &pos) : Node(NodeType::exit_statement, pos) {}
  ~ExitStatementNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_EXITSTATEMENTNODE_H
