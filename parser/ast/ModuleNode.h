#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H

#include "DeclarationSequenceNode.h"
#include "ImportListNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class ModuleNode final : public Node {
public:
  ModuleNode(const FilePos &pos) : Node(NodeType::module, pos) {}
  ~ModuleNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::string ident;
  std::unique_ptr<const ImportListNode> import_list;
  std::unique_ptr<DeclarationSequenceNode> declaration_sequence;
  std::unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_MODULENODE_H
