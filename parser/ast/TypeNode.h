#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "IdentNode.h"
#include "Node.h"
#include "parser/ast/FPSectionNode.h"
#include <memory>

using std::string;
using std::unique_ptr;

class ExpressionNode;
class TypeNode : public Node {
public:
  TypeNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~TypeNode() override = default;
};

class IdentTypeNode final : public TypeNode {
public:
  IdentTypeNode(const FilePos &pos, unique_ptr<IdentNode> ident)
      : TypeNode(NodeType::ident_type, pos), ident(std::move(ident)) {}
  ~IdentTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
};

class ArrayTypeNode final : public TypeNode {
public:
  ArrayTypeNode(const FilePos &pos, unique_ptr<ExpressionNode> expression,
                unique_ptr<TypeNode> type)
      : TypeNode(NodeType::array_type, pos), expression(std::move(expression)),
        type(std::move(type)) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> expression;
  const unique_ptr<TypeNode> type;
};

class FieldNode final : public Node {
public:
  FieldNode(const FilePos &pos, unique_ptr<IdentNode> ident, TypeNode *type)
      : Node(NodeType::field, pos), ident(std::move(ident)), type(type) {}
  ~FieldNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
  const TypeNode *type;
};

class RecordTypeNode final : public TypeNode {
public:
  RecordTypeNode(const FilePos &pos, vector<unique_ptr<FieldNode>> &field_lists)
      : TypeNode(NodeType::record_type, pos), field_lists(field_lists) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const vector<unique_ptr<FieldNode>> &field_lists;
};

#endif // OBERON0C_TYPENODE_H
