#ifndef OBERON0C_DECLARATIONSEQUENCENODE_H
#define OBERON0C_DECLARATIONSEQUENCENODE_H

#include "FPSectionNode.h"
#include "Node.h"
#include "StatementSequenceNode.h"
#include "TypeNode.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class ConstDeclarationNode final : public Node {
public:
  ConstDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> &ident,
                       unique_ptr<ExpressionNode> &expression)
      : Node(NodeType::const_declaration, pos), ident(ident),
        expression(expression) {}
  ~ConstDeclarationNode() final = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> &ident;
  const unique_ptr<ExpressionNode> &expression;
};

class TypeDeclarationNode final : public Node {
public:
  TypeDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> &ident,
                      unique_ptr<TypeNode> &type)
      : Node(NodeType::type_declaration, pos), ident(ident), type(type) {}
  ~TypeDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> &ident;
  const unique_ptr<TypeNode> &type;
};

class VarDeclarationNode final : public Node {
public:
  VarDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> &ident,
                     TypeNode *type)
      : Node(NodeType::var_declaration, pos), ident(ident), type(type) {}
  ~VarDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<IdentNode> &ident;
  const TypeNode *type;
};

class ProcedureDeclarationNode;
class DeclarationSequence final {
public:
  DeclarationSequence(vector<unique_ptr<ConstDeclarationNode>> &_consts,
                      vector<unique_ptr<TypeDeclarationNode>> &_types,
                      vector<unique_ptr<VarDeclarationNode>> &_vars,
                      vector<unique_ptr<ProcedureDeclarationNode>> &_procs)
      : consts(_consts), types(_types), vars(_vars), procs(_procs) {}

  const vector<unique_ptr<ConstDeclarationNode>> &consts;
  const vector<unique_ptr<TypeDeclarationNode>> &types;
  const vector<unique_ptr<VarDeclarationNode>> &vars;
  const vector<unique_ptr<ProcedureDeclarationNode>> &procs;
};

class DeclarationSequenceNode : public Node {
public:
  DeclarationSequenceNode(const NodeType &type, const FilePos &pos,
                          DeclarationSequence decl)
      : Node(type, pos), constants(decl.consts), types(decl.types),
        variables(decl.vars), procedures(decl.procs) {};
  ~DeclarationSequenceNode() override = default;

  const vector<unique_ptr<ConstDeclarationNode>> &constants;
  const vector<unique_ptr<TypeDeclarationNode>> &types;
  const vector<unique_ptr<VarDeclarationNode>> &variables;
  const vector<unique_ptr<ProcedureDeclarationNode>> &procedures;
};

class ProcedureDeclarationNode final : public DeclarationSequenceNode {
public:
  ProcedureDeclarationNode(
      const FilePos &pos, unique_ptr<IdentNode> &proc_name,
      vector<unique_ptr<FPSectionNode>> &formal_parameters,
      DeclarationSequence decl,
      unique_ptr<StatementSequenceNode> &statement_sequence)
      : DeclarationSequenceNode(NodeType::procedure_declaration, pos, decl),
        proc_name(proc_name), formal_parameters(formal_parameters),
        statement_sequence(statement_sequence) {}
  ~ProcedureDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  // ProcedureHeading
  const unique_ptr<IdentNode> &proc_name;
  const vector<unique_ptr<FPSectionNode>> &formal_parameters;
  // ProcedureBody
  const unique_ptr<StatementSequenceNode> &statement_sequence;
};

#endif // OBERON0C_DECLARATIONSEQUENCENODE_H
