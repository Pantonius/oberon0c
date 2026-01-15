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

class DeclarationNode : public Node {
private:
public:
  DeclarationNode(const NodeType &type, const FilePos &pos,
                  unique_ptr<IdentNode> ident, TypeNode *type_node)
      : Node(type, pos), ident(std::move(ident)), type(type_node) {}
  ~DeclarationNode() override = default;

  const unique_ptr<IdentNode> ident;
  TypeNode *type;
};

class ConstDeclarationNode final : public DeclarationNode {
public:
  ConstDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> ident,
                       unique_ptr<ExpressionNode> expression,
                       TypeNode *type_node)
      : DeclarationNode(NodeType::const_declaration, pos, std::move(ident),
                        type_node),
        expression(std::move(expression)) {}
  ~ConstDeclarationNode() final = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  const unique_ptr<ExpressionNode> expression;
};

class TypeDeclarationNode final : public DeclarationNode {
public:
  TypeDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> ident,
                      TypeNode *type_node)
      : DeclarationNode(NodeType::type_declaration, pos, std::move(ident),
                        type_node) {}
  ~TypeDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
};

class VarDeclarationNode final : public DeclarationNode {
public:
  VarDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> ident,
                     TypeNode *type_node)
      : DeclarationNode(NodeType::var_declaration, pos, std::move(ident),
                        type_node) {}
  ~VarDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;
};

class ProcedureDeclarationNode;

class DeclarationSequenceNode {
private:
  vector<unique_ptr<ConstDeclarationNode>> constants_;
  vector<unique_ptr<TypeDeclarationNode>> types_;
  vector<unique_ptr<VarDeclarationNode>> variables_;
  vector<unique_ptr<ProcedureDeclarationNode>> procedures_;

public:
  DeclarationSequenceNode() {};
  ~DeclarationSequenceNode() = default;

  void add_const(unique_ptr<ConstDeclarationNode> con);
  vector<unique_ptr<ConstDeclarationNode>> get_consts();

  void add_type(unique_ptr<TypeDeclarationNode> type);
  vector<unique_ptr<TypeDeclarationNode>> get_types();

  void add_var(unique_ptr<VarDeclarationNode> var);
  vector<unique_ptr<VarDeclarationNode>> get_vars();

  void add_procedure(unique_ptr<ProcedureDeclarationNode> proc);
  vector<unique_ptr<ProcedureDeclarationNode>> get_procs();
};

class ProcedureDeclarationNode final : public DeclarationNode,
                                       public DeclarationSequenceNode {
private:
  unique_ptr<StatementSequenceNode> statement_sequence_;

public:
  ProcedureDeclarationNode(const FilePos &pos, unique_ptr<IdentNode> proc_name,
                           vector<unique_ptr<FPSectionNode>> &formal_parameters,
                           TypeNode *type_node)
      : DeclarationNode(NodeType::procedure_declaration, pos,
                        std::move(proc_name), type_node),
        formal_parameters(formal_parameters) {}
  ~ProcedureDeclarationNode() override = default;

  void accept(NodeVisitor &visitor) final;
  void print(std::ostream &stream) const final;

  // ProcedureHeading
  const vector<unique_ptr<FPSectionNode>> &formal_parameters;

  void set_statement_sequence(unique_ptr<StatementSequenceNode>);
};

#endif // OBERON0C_DECLARATIONSEQUENCENODE_H
