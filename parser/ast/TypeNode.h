#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "IdentNode.h"
#include "Node.h"
#include "global.h"
#include <memory>
#include <ostream>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class ExpressionNode;
class VarDeclarationNode;
class ParamDeclarationNode;
class TypeNode : public Node {
public:
  TypeNode(const NodeType &type, const FilePos &pos) : Node(type, pos) {}
  ~TypeNode() override = default;

  bool is_equivalent(TypeNode *) const;
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
                const TypeNode *type)
      : TypeNode(NodeType::array_type, pos), expression(std::move(expression)),
        type(type) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> expression;
  const TypeNode *type;
};

class FieldNode final : public Node {
public:
  FieldNode(const FilePos &pos, unique_ptr<IdentNode> ident,
            const TypeNode *type)
      : Node(NodeType::field, pos), ident(std::move(ident)), type(type) {}
  ~FieldNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
  const TypeNode *type;
};

class RecordTypeNode final : public TypeNode {
public:
  RecordTypeNode(const FilePos &pos,
                 std::vector<unique_ptr<VarDeclarationNode>> field_lists)
      : TypeNode(NodeType::record_type, pos),
        field_lists(std::move(field_lists)) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const VarDeclarationNode *find_field(const IdentNode &ident) const;

  const std::vector<unique_ptr<VarDeclarationNode>> field_lists;
};

class FieldNotFoundException : public std::exception {
private:
  const IdentNode &ident_;
  const string msg;

public:
  FieldNotFoundException(const IdentNode &ident)
      : ident_(ident),
        msg("Record does not have a field '" + to_string(ident_) + "'") {}

  const char *what() const noexcept override { return msg.c_str(); }
  const IdentNode &field() const noexcept { return ident_; }
};

class ProcedureTypeNode final : public TypeNode {
public:
  ProcedureTypeNode(const FilePos &pos,
                    vector<unique_ptr<ParamDeclarationNode>> formal_parameters)
      : TypeNode(NodeType::procedure_type, pos),
        formal_parameters(std::move(formal_parameters)) {}
  ~ProcedureTypeNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  vector<unique_ptr<ParamDeclarationNode>> formal_parameters;
};

#endif // OBERON0C_TYPENODE_H
