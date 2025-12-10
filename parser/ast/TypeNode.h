#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "ExpressionNode.h"
#include "Node.h"
#include <memory>

using std::string;
using std::unique_ptr;

class TypeNode;
class ArrayTypeNode final : public Node {
public:
  ArrayTypeNode(const FilePos &pos) : Node(NodeType::array_type, pos) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ExpressionNode> expression;
  unique_ptr<TypeNode> type;
};

class RecordTypeNode final : public Node {
public:
  RecordTypeNode(const FilePos &pos) : Node(NodeType::record_type, pos) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  vector<unique_ptr<pair<string, TypeNode>>> field_lists;
};

class TypeNode final : public Node {
public:
  TypeNode(const FilePos &pos) : Node(NodeType::type, pos) {}
  ~TypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ArrayTypeNode> array_type;
  unique_ptr<RecordTypeNode> record_type;
};

#endif // OBERON0C_TYPENODE_H
