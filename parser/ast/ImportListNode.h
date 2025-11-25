#ifndef OBERON0C_IMPORTLISTNODE_H
#define OBERON0C_IMPORTLISTNODE_H

#include "ImportNode.h"
#include "Node.h"
#include <memory>
#include <vector>

class ImportListNode final : public Node {
public:
  ImportListNode(const FilePos &pos) : Node(NodeType::import_list, pos) {}
  ~ImportListNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  std::vector<std::unique_ptr<const ImportNode>> list;
};

#endif // OBERON0C_IMPORTLISTNODE_H
