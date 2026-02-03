#include "TypeNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void IdentTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IdentTypeNode::print(ostream &stream) const { ident->print(stream); }

void ArrayTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ArrayTypeNode::print(ostream &stream) const {
  stream << "ARRAY ";
  expression->print(stream);
  stream << " OF ";
  type->print(stream);
}

bool ArrayTypeNode::is_in_bounds(const NumberExpressionNode *expr) const {
  return expr->value >= 0 && expr->value < expression->value;
}

void FieldNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void FieldNode::print(ostream &stream) const {
  ident->print(stream);
  stream << " : ";
  type->print(stream);
}

void RecordTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void RecordTypeNode::print(ostream &stream) const {
  stream << "RECORD ";

  auto fl_size = field_lists.size();
  if (fl_size > 0) {
    field_lists[0]->print(stream);

    for (size_t i = 1; i < fl_size; i++) {
      stream << "; ";
      field_lists[i]->print(stream);
    }
  }
  stream << " END";
}

const VarDeclarationNode *
RecordTypeNode::find_field(const IdentNode &ident) const {
  for (auto &field : field_lists) {
    if (ident.value == field->ident->value) {
      return field.get();
    }
  }

  throw FieldNotFoundException(ident);
}

void ProcedureTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ProcedureTypeNode::print(ostream &stream) const { stream << "PROCEDURE"; }
