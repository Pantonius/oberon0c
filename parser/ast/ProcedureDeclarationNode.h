#ifndef OBERON0C_PROCEDUREDECLARATIONNODE_H
#define OBERON0C_PROCEDUREDECLARATIONNODE_H

#include "Node.h"
#include "ProcedureBodyNode.h"
#include "ProcedureHeadingNode.h"
#include <memory>

using std::unique_ptr;

class ProcedureDeclarationNode final : public Node {
public:
  ProcedureDeclarationNode(const FilePos &pos)
      : Node(NodeType::procedure_declaration, pos) {}
  ~ProcedureDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  unique_ptr<ProcedureHeadingNode> heading;
  unique_ptr<ProcedureBodyNode> body;
  string ident;
};

#endif // OBERON0C_PROCEDUREDECLARATIONNODE_H
