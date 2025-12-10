#ifndef OBERON0C_DECLARATIONSEQUENCENODE_H
#define OBERON0C_DECLARATIONSEQUENCENODE_H

#include "Node.h"
#include "ProcedureBodyNode.h"
#include "ProcedureHeadingNode.h"
#include "TypeNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class ConstDeclarationNode final : public Node {
public:
  ConstDeclarationNode(const FilePos &pos)
      : Node(NodeType::const_declaration, pos) {}
  ~ConstDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
};

class ProcedureDeclarationNode final : public Node {
public:
  ProcedureDeclarationNode(const FilePos &pos)
      : Node(NodeType::procedure_declaration, pos) {}
  ~ProcedureDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  unique_ptr<ProcedureHeadingNode> heading;
  unique_ptr<ProcedureBodyNode> body;
  string ident;
};

class TypeDeclarationNode final : public Node {
public:
  TypeDeclarationNode(const FilePos &pos)
      : Node(NodeType::type_declaration, pos) {}
  ~TypeDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<TypeNode> type;
};

class VarDeclarationNode final : public Node {
public:
  VarDeclarationNode(const FilePos &pos)
      : Node(NodeType::var_declaration, pos) {}
  ~VarDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  string ident;
  unique_ptr<ExpressionNode> expression;
  string type;
};

class DeclarationSequenceNode final : public Node {
public:
  DeclarationSequenceNode(const FilePos &pos)
      : Node(NodeType::declaration_sequence, pos) {}
  ~DeclarationSequenceNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  vector<unique_ptr<ConstDeclarationNode>> constants;
  vector<unique_ptr<TypeDeclarationNode>> types;
  vector<unique_ptr<VarDeclarationNode>> variables;
  vector<unique_ptr<ProcedureDeclarationNode>> procedures;
};

#endif // OBERON0C_DECLARATIONSEQUENCENODE_H
