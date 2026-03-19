#include "DeclarationSequenceNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void DeclarationSequenceNode::add_const(
    unique_ptr<ConstDeclarationNode> const_decl) {
  constants_.push_back(std::move(const_decl));
}

vector<unique_ptr<ConstDeclarationNode>> *
DeclarationSequenceNode::get_consts() {
  return &constants_;
}

void DeclarationSequenceNode::add_type(
    unique_ptr<TypeDeclarationNode> type_decl) {
  types_.push_back(std::move(type_decl));
}

vector<unique_ptr<TypeDeclarationNode>> *DeclarationSequenceNode::get_types() {
  return &types_;
}

void DeclarationSequenceNode::add_var(unique_ptr<VarDeclarationNode> var_decl) {
  variables_.push_back(std::move(var_decl));
}

vector<unique_ptr<VarDeclarationNode>> *DeclarationSequenceNode::get_vars() {
  return &variables_;
}

void DeclarationSequenceNode::add_procedure(
    unique_ptr<ProcedureDeclarationNode> proc_decl) {
  procedures_.push_back(std::move(proc_decl));
}

vector<unique_ptr<ProcedureDeclarationNode>> *
DeclarationSequenceNode::get_procs() {
  return &procedures_;
}

void ConstDeclarationNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}

void ConstDeclarationNode::print(ostream &stream) const {
  stream << "CONST ";
  ident->print(stream);
  stream << " = ";
  expression->print(stream);
}

void VarDeclarationNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void VarDeclarationNode::print(ostream &stream) const {
  stream << "VAR ";
  ident->print(stream);
  stream << " : ";
  type->print(stream);
}

void ParamDeclarationNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void ParamDeclarationNode::print(ostream &stream) const {
  if (this->by_reference)
    stream << "VAR ";

  ident->print(stream);
  stream << " : ";
  type->print(stream);
}

void TypeDeclarationNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void TypeDeclarationNode::print(ostream &stream) const {
  stream << "TYPE ";
  ident->print(stream);
  stream << " : ";
  type->print(stream);
}

void ProcedureDeclarationNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void ProcedureDeclarationNode::print(ostream &stream) const {
  stream << "PROCEDURE ";
  ident->print(stream);
  // stream << "(";
  //
  // int fp_size = type.size();
  // if (fp_size > 0) {
  //   formal_parameters[0]->print(stream);
  //
  //   for (size_t i = 1; i < formal_parameters.size(); i++) {
  //     stream << ", ";
  //     formal_parameters[i]->print(stream);
  //   }
  // }
  //
  // stream << ")";
  stream << "\n";
  statement_sequence_->print(stream);
  stream << "END ";
  ident->print(stream);
}
void ProcedureDeclarationNode::set_statements(
    unique_ptr<StatementSequenceNode> stmt_seq) {
  statement_sequence_ = std::move(stmt_seq);
}

StatementSequenceNode *ProcedureDeclarationNode::get_statements() {
  return statement_sequence_.get();
}
