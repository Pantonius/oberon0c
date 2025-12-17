#ifndef OBERON0C_NODEVISITOR_H
#define OBERON0C_NODEVISITOR_H

#include "ModuleNode.h"

class NodeVisitor {

public:
  explicit NodeVisitor() = default;
  virtual ~NodeVisitor() noexcept;

  virtual void visit(ActualParametersNode &actual_parameters) = 0;
  virtual void visit(ArrayTypeNode &array_type) = 0;
  virtual void visit(AssignmentNode &assignment) = 0;
  virtual void visit(ConstDeclarationNode &const_declaration) = 0;
  virtual void visit(DeclarationSequenceNode &declaration_sequence) = 0;
  virtual void visit(ExpressionNode &expression) = 0;
  virtual void visit(FormalParametersNode &formal_parameters) = 0;
  virtual void visit(FPSectionNode &fp_section) = 0;
  virtual void visit(IfStatementNode &if_statement) = 0;
  virtual void visit(ModuleNode &module_node) = 0;
  virtual void visit(ProcedureCallNode &procedure_call) = 0;
  virtual void visit(ProcedureDeclarationNode &procedure_declaration) = 0;
  virtual void visit(RecordTypeNode &record_type) = 0;
  virtual void visit(RepeatStatementNode &repeat_statement) = 0;
  virtual void visit(SelectorNode &selector) = 0;
  virtual void visit(StatementNode &statement) = 0;
  virtual void visit(StatementSequenceNode &statement_sequence) = 0;
  virtual void visit(TypeNode &type) = 0;
  virtual void visit(TypeDeclarationNode &type_declaration) = 0;
  virtual void visit(VarDeclarationNode &var_declaration) = 0;
  virtual void visit(WhileStatementNode &while_statement) = 0;
};

#endif // OBERON0C_NODEVISITOR_H
