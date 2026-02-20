#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "IdentNode.h"
#include "Node.h"
#include "global.h"
#include <memory>
#include <optional>
#include <ostream>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class ExpressionNode;
class NumberExpressionNode;
class VarDeclarationNode;
class ParamDeclarationNode;

enum class StdType : char { BOOLEAN, INTEGER };

class TypeNode : public Node {
public:
  TypeNode(const NodeType &type, const FilePos pos) : Node(type, pos) {}
  ~TypeNode() override = default;

  bool operator==(const TypeNode &) const = default;
};

class IdentTypeNode final : public TypeNode {
public:
  IdentTypeNode(const FilePos pos, unique_ptr<IdentNode> ident)
      : TypeNode(NodeType::ident_type, pos), ident(std::move(ident)) {}
  ~IdentTypeNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
};

class StdTypeNode final : public TypeNode {
public:
  StdTypeNode(const FilePos pos, unique_ptr<IdentNode> ident,
              const StdType std_type)
      : TypeNode(NodeType::std_type, pos), ident(std::move(ident)),
        std_type(std_type) {}
  ~StdTypeNode() override = default;

  bool operator==(const StdTypeNode &) const = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
  const StdType std_type;
};

template <> struct std::hash<StdTypeNode> {
  std::size_t operator()(const StdTypeNode &s) const noexcept {
    return std::hash<std::string>{}(s.ident->value);
  }
};

class ArrayTypeNode final : public TypeNode {
public:
  ArrayTypeNode(const FilePos pos, unique_ptr<NumberExpressionNode> expression,
                TypeNode *type)
      : TypeNode(NodeType::array_type, pos), expression(std::move(expression)),
        type(type) {}
  ~ArrayTypeNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<NumberExpressionNode> expression;
  TypeNode *type;

  std::optional<bool> is_in_bounds(const ExpressionNode *expr) const;
};

class FieldNode final : public Node {
public:
  FieldNode(const FilePos pos, unique_ptr<IdentNode> ident,
            const TypeNode *type)
      : Node(NodeType::field, pos), ident(std::move(ident)), type(type) {}
  ~FieldNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> ident;
  const TypeNode *type;
};

class RecordTypeNode final : public TypeNode {
public:
  RecordTypeNode(const FilePos pos,
                 std::vector<unique_ptr<VarDeclarationNode>> field_lists)
      : TypeNode(NodeType::record_type, pos),
        field_lists(std::move(field_lists)) {}
  ~RecordTypeNode() override = default;

  void accept(NodeVisitor &visitor) override final;
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
  ProcedureTypeNode(
      const FilePos pos,
      vector<unique_ptr<ParamDeclarationNode>> formal_parameters = {})
      : TypeNode(NodeType::procedure_type, pos),
        formal_parameters(std::move(formal_parameters)) {}
  ~ProcedureTypeNode() override = default;

  void accept(NodeVisitor &visitor) override final;
  void print(std::ostream &stream) const final;

  vector<unique_ptr<ParamDeclarationNode>> formal_parameters;
};

#endif // OBERON0C_TYPENODE_H
