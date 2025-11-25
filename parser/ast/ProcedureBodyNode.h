#ifndef OBERON0C_PROCEDUREBODYNODE_H
#define OBERON0C_PROCEDUREBODYNODE_H

#include "DeclarationSequenceNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class ProcedureBodyNode final : public Node {
public:
  ProcedureBodyNode(const FilePos &pos) : Node(NodeType::procedure_body, pos) {}
  ~ProcedureBodyNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<DeclarationSequenceNode> declarations;
  std::unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_PROCEDUREBODYNODE_H
