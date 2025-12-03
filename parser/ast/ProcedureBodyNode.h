#ifndef OBERON0C_PROCEDUREBODYNODE_H
#define OBERON0C_PROCEDUREBODYNODE_H

#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>

using std::unique_ptr;

class DeclarationSequenceNode;

class ProcedureBodyNode final : public Node {
public:
  ProcedureBodyNode(const FilePos &pos) : Node(NodeType::procedure_body, pos) {}
  ~ProcedureBodyNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<DeclarationSequenceNode> declarations;
  unique_ptr<StatementSequenceNode> statement_sequence;
  string end_ident;
};

#endif // OBERON0C_PROCEDUREBODYNODE_H
