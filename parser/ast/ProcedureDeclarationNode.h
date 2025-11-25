#ifndef OBERON0C_PROCEDUREDECLARATIONNODE_H
#define OBERON0C_PROCEDUREDECLARATIONNODE_H

#include "Node.h"
#include "ProcedureBodyNode.h"
#include "ProcedureHeadingNode.h"
#include <memory>
#include <vector>

class ProcedureDeclarationNode final : public Node {
public:
  ProcedureDeclarationNode(const FilePos &pos)
      : Node(NodeType::procedure_declaration, pos) {}
  ~ProcedureDeclarationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::unique_ptr<ProcedureHeadingNode> heading;
  std::unique_ptr<ProcedureBodyNode> body;
  string ident;
};

#endif // OBERON0C_PROCEDUREDECLARATIONNODE_H
