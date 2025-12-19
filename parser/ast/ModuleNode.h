#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H

#include "DeclarationSequenceNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>

using std::string;
using std::unique_ptr;

class ModuleNode final : public DeclarationSequenceNode {
public:
  ModuleNode(const FilePos &pos, unique_ptr<IdentNode> &ident,
             DeclarationSequence decl, unique_ptr<StatementSequenceNode> &stmts)
      : DeclarationSequenceNode(NodeType::module, pos, decl), ident(ident),
        statement_sequence(stmts) {}
  ~ModuleNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  const unique_ptr<IdentNode> &ident;
  const unique_ptr<StatementSequenceNode> &statement_sequence;
};

#endif // OBERON0C_MODULENODE_H
