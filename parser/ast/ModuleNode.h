#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H

#include "DeclarationSequenceNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>

using std::string;
using std::unique_ptr;

class ModuleNode final : public Node {
public:
  ModuleNode(const FilePos &pos) : Node(NodeType::module, pos) {}
  ~ModuleNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<DeclarationSequenceNode> declaration_sequence;
  unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_MODULENODE_H
