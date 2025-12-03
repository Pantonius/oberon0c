#ifndef OBERON0C_RECORDTYPENODE_H
#define OBERON0C_RECORDTYPENODE_H

#include "Node.h"
#include "TypeNode.h"

using std::string;

class RecordTypeNode final : public Node {
public:
  RecordTypeNode(const FilePos &pos) : Node(NodeType::record_type, pos) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  vector<unique_ptr<pair<string, TypeNode>>> field_lists;
};

#endif // OBERON0C_RECORDTYPENODE_H
