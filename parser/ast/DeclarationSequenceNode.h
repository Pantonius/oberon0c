#ifndef OBERON0C_DECLARATIONSEQUENCE_H
#define OBERON0C_DECLARATIONSEQUENCE_H

#include "ConstDeclarationNode.h"
#include "Node.h"
#include "ProcedureDeclarationNode.h"
#include "TypeDeclarationNode.h"
#include "VarDeclarationNode.h"
#include <memory>
#include <vector>

class DeclarationSequenceNode final : public Node {
public:
  DeclarationSequenceNode(const FilePos &pos) : Node(NodeType::import, pos) {}
  ~DeclarationSequenceNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::vector<std::unique_ptr<ConstDeclarationNode>> constants;
  std::vector<std::unique_ptr<TypeDeclarationNode>> types;
  std::vector<std::unique_ptr<VarDeclarationNode>> variables;
  std::vector<std::unique_ptr<ProcedureDeclarationNode>> procedures;
};

#endif // OBERON0C_DECLARATIONSEQUENCE_H
