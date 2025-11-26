#ifndef OBERON0C_SETNODE_H
#define OBERON0C_SETNODE_H

#include "ElementNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class SetNode final : public Node {
public:
  SetNode(const FilePos &pos) : Node(NodeType::set, pos) {}
  ~SetNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<ElementNode>> elements;
};

#endif // OBERON0C_SETNODE_H
