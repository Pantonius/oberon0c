#ifndef OBERON0C_PROCEDUREDECLARATIONNODE_H
#define OBERON0C_PROCEDUREDECLARATIONNODE_H

#include "Node.h"
#include <memory>
#include <vector>

class ProcedureDeclarationNode final : public Node {
public:
  ProcedureDeclarationNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~ProcedureDeclarationNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;
};

#endif // OBERON0C_PROCEDUREDECLARATIONNODE_H
