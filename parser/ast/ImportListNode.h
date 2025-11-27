#ifndef OBERON0C_IMPORTLISTNODE_H
#define OBERON0C_IMPORTLISTNODE_H

#include "ImportNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class ImportListNode final : public Node {
public:
  ImportListNode(const FilePos &pos) : Node(NodeType::import_list, pos) {}
  ~ImportListNode() override = default;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<ImportNode>> list;
};

#endif // OBERON0C_IMPORTLISTNODE_H
