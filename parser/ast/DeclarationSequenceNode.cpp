#include "DeclarationSequenceNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

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

void TypeDeclarationNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void TypeDeclarationNode::print(ostream &stream) const {
  stream << "TYPE ";
  ident->print(stream);
  stream << " = ";
  type->print(stream);
}

void ProcedureDeclarationNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void ProcedureDeclarationNode::print(ostream &stream) const {
  stream << "PROCEDURE ";
  proc_name->print(stream);
  stream << "(";

  int fp_size = formal_parameters.size();
  if (fp_size > 0) {
    formal_parameters[0]->print(stream);

    for (size_t i = 1; i < formal_parameters.size(); i++) {
      stream << ", ";
      formal_parameters[i]->print(stream);
    }
  }

  stream << ")";
  statement_sequence->print(stream);
  stream << "END ";
  proc_name->print(stream);
}
