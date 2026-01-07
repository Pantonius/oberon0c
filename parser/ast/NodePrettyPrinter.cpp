#include "NodePrettyPrinter.h"

void NodePrettyPrinter::visit(ArrayTypeNode &array_type) {}
void NodePrettyPrinter::visit(AssignmentNode &assignment) {}
void NodePrettyPrinter::visit(ConstDeclarationNode &const_declaration) {
  stream_ << "CONST " << const_declaration.ident->value << " = ";
  const_declaration.expression->accept(*this);
  stream_ << ";" << std::endl;
}
void NodePrettyPrinter::declaration_sequence(
    DeclarationSequenceNode &declaration_sequence) {
  for (size_t i = 0; i < declaration_sequence.constants.size(); i++) {
    declaration_sequence.constants[i]->accept(*this);
  }
  for (size_t i = 0; i < declaration_sequence.variables.size(); i++) {
    declaration_sequence.variables[i]->accept(*this);
  }
  for (size_t i = 0; i < declaration_sequence.types.size(); i++) {
    declaration_sequence.types[i]->accept(*this);
  }
  for (size_t i = 0; i < declaration_sequence.procedures.size(); i++) {
    declaration_sequence.procedures[i]->accept(*this);
  }
}
void NodePrettyPrinter::visit(ExpressionNode &expression) {}
void NodePrettyPrinter::visit(FPSectionNode &fp_section) {}
void NodePrettyPrinter::visit(IfStatementNode &if_statement) {}
void NodePrettyPrinter::visit(ModuleNode &module_node) {
  stream_ << "MODULE " << module_node.ident->value;
  stream_ << "END" << module_node.ident->value << ";";
}
void NodePrettyPrinter::visit(ProcedureCallNode &procedure_call) {}
void NodePrettyPrinter::visit(ProcedureDeclarationNode &procedure_declaration) {
}
void NodePrettyPrinter::visit(RecordTypeNode &record_type) {}
void NodePrettyPrinter::visit(RepeatStatementNode &repeat_statement) {}
void NodePrettyPrinter::visit(SelectorNode &selector) {}
void NodePrettyPrinter::visit(StatementNode &statement) {}
void NodePrettyPrinter::visit(StatementSequenceNode &statement_sequence) {}
void NodePrettyPrinter::visit(TypeNode &type) {}
void NodePrettyPrinter::visit(TypeDeclarationNode &type_declaration) {}
void NodePrettyPrinter::visit(VarDeclarationNode &var_declaration) {}
void NodePrettyPrinter::visit(WhileStatementNode &while_statement) {}
