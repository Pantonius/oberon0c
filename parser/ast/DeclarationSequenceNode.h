#ifndef OBERON0C_DECLARATIONSEQUENCENODE_H
#define OBERON0C_DECLARATIONSEQUENCENODE_H

#include "FormalParametersNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
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

class ProcedureDeclarationNode;
class DeclarationSequenceNode : public Node {
public:
  DeclarationSequenceNode(const NodeType &type, const FilePos &pos)
      : Node(type, pos) {};
  ~DeclarationSequenceNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  vector<unique_ptr<ConstDeclarationNode>> &constants();
  vector<unique_ptr<TypeDeclarationNode>> &types();
  vector<unique_ptr<VarDeclarationNode>> &variables();
  vector<unique_ptr<ProcedureDeclarationNode>> &procedures();

private:
  vector<unique_ptr<ConstDeclarationNode>> consts_;
  vector<unique_ptr<TypeDeclarationNode>> types_;
  vector<unique_ptr<VarDeclarationNode>> vars_;
  vector<unique_ptr<ProcedureDeclarationNode>> procs_;
};

class ProcedureDeclarationNode final : public DeclarationSequenceNode {
public:
  ProcedureDeclarationNode(const FilePos &pos)
      : DeclarationSequenceNode(NodeType::procedure_declaration, pos) {}
  ~ProcedureDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) override {};
  void print(std::ostream &stream) const override {};

  // ProcedureHeading
  string proc_name;
  unique_ptr<FormalParametersNode> formal_parameters;
  // ProcedureBody
  unique_ptr<StatementSequenceNode> statement_sequence;
};

#endif // OBERON0C_DECLARATIONSEQUENCENODE_H
