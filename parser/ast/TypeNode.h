#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "ExpressionNode.h"
#include "IdentNode.h"
#include "Node.h"
#include <memory>

using std::string;
using std::unique_ptr;

class TypeNode : public Node {
public:
  TypeNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~TypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};
};

class IdentTypeNode final : public TypeNode {
public:
  IdentTypeNode(const FilePos &pos, unique_ptr<IdentNode> &ident)
      : TypeNode(NodeType::ident_type, pos), ident(ident) {}
  ~IdentTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  const unique_ptr<IdentNode> &ident;
};

class ArrayTypeNode final : public TypeNode {
public:
  ArrayTypeNode(const FilePos &pos, unique_ptr<ExpressionNode> &expression,
                unique_ptr<TypeNode> &type)
      : TypeNode(NodeType::array_type, pos), expression(expression),
        type(type) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  const unique_ptr<ExpressionNode> &expression;
  const unique_ptr<TypeNode> &type;
};

class FieldNode final : public Node {
public:
  FieldNode(const FilePos &pos, unique_ptr<IdentNode> &ident, TypeNode *type)
      : Node(NodeType::field, pos), ident(ident), type(type) {}
  ~FieldNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  const unique_ptr<IdentNode> &ident;
  const TypeNode *type;
};

class RecordTypeNode final : public TypeNode {
public:
  RecordTypeNode(const FilePos &pos, vector<unique_ptr<FieldNode>> &field_lists)
      : TypeNode(NodeType::record_type, pos), field_lists(field_lists) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  const vector<unique_ptr<FieldNode>> &field_lists;
};

#endif // OBERON0C_TYPENODE_H
