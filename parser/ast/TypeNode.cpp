#include "TypeNode.h"
#include "NodeVisitor.h"
#include "parser/ast/ExpressionNode.h"
#include "util/Logger.h"
#include <optional>

void IdentTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IdentTypeNode::print(ostream &stream) const { ident->print(stream); }

void ArrayTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ArrayTypeNode::print(ostream &stream) const {
  stream << "ARRAY " << expression->value << " OF ";
  type->print(stream);
}

std::optional<bool>
ArrayTypeNode::is_in_bounds(const ExpressionNode *expr) const {
  if (auto number_expr = dynamic_cast<const NumberExpressionNode *>(expr)) {
    return number_expr->value >= 0 && number_expr->value < expression->value;
  }

  return {};
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

size_t RecordTypeNode::find_field_index(const IdentNode &ident) const {
  for (size_t i = 0; i < field_lists.size(); i++) {
    if (ident.value == field_lists.at(i)->ident->value) {
      return i;
    }
  }

  throw FieldNotFoundException(ident);
}

void ProcedureTypeNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ProcedureTypeNode::print(ostream &stream) const { stream << "PROCEDURE"; }
