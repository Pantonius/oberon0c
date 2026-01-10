#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H

#include "DeclarationSequenceNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>

using std::string;
using std::unique_ptr;

class ModuleNode final : public Node, public DeclarationSequenceNode {
private:
  unique_ptr<StatementSequenceNode> statement_sequence_;

public:
  ModuleNode(const FilePos &pos, unique_ptr<IdentNode> ident)
      : Node(NodeType::module, pos), ident(std::move(ident)) {}
  ~ModuleNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
};

#endif // OBERON0C_MODULENODE_H
