#ifndef OBERON0C_DECLARATIONSEQUENCENODE_H
#define OBERON0C_DECLARATIONSEQUENCENODE_H

#include "ConstDeclarationNode.h"
#include "Node.h"
#include "ProcedureDeclarationNode.h"
#include "TypeDeclarationNode.h"
#include "VarDeclarationNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class DeclarationSequenceNode final : public Node {
public:
  DeclarationSequenceNode(const FilePos &pos)
      : Node(NodeType::declaration_sequence, pos) {}
  ~DeclarationSequenceNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<ConstDeclarationNode>> constants;
  vector<unique_ptr<TypeDeclarationNode>> types;
  vector<unique_ptr<VarDeclarationNode>> variables;
  vector<unique_ptr<ProcedureDeclarationNode>> procedures;
};

#endif // OBERON0C_DECLARATIONSEQUENCENODE_H
