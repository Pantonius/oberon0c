#include "StatementNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void AssignmentNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void AssignmentNode::print(ostream &stream) const {
  ident->print(stream);

  for (size_t i = 0; i < selectors.size(); i++) {
    selectors[i]->print(stream);
  }

  stream << " = ";
  expression->print(stream);
}

void ElsIfStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ElsIfStatementNode::print(ostream &stream) const {
  stream << "ELSIF ";
  condition->print(stream);
  stream << "\n";
  body->print(stream);
  stream << "END";
}

void IfStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IfStatementNode::print(ostream &stream) const {
  stream << "IF ";
  condition->print(stream);
  stream << "\n";
  body->print(stream);
  stream << "END";
}

void ProcedureCallNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ProcedureCallNode::print(ostream &stream) const {
  ident->print(stream);

  for (size_t i = 0; i < selectors.size(); i++) {
    selectors[i]->print(stream);
  }

  stream << "(";

  auto ap_size = actual_parameters.size();
  if (ap_size > 0) {
    actual_parameters[0]->print(stream);

    for (size_t i = 1; i < ap_size; i++) {
      stream << ", ";
      actual_parameters[i]->print(stream);
    }
  }
  stream << ")";
}

void WhileStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void WhileStatementNode::print(ostream &stream) const {
  stream << "WHILE ";
  condition->print(stream);
  stream << "DO\n";
  body->print(stream);
  stream << "\nEND";
}

void RepeatStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void RepeatStatementNode::print(ostream &stream) const {
  stream << "REPEAT\n";
  body->print(stream);
  stream << "\nUNTIL ";
  condition->print(stream);
}
