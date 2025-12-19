#ifndef OBERON0C_NODEPRETTYPRINER_H
#define OBERON0C_NODEPRETTYPRINER_H

#include "NodeVisitor.h"

class NodePrettyPrinter : private NodeVisitor {

public:
  explicit NodePrettyPrinter() = default;
  virtual ~NodePrettyPrinter() noexcept override;

  void visit(ArrayTypeNode &array_type) override;
  void visit(AssignmentNode &assignment) override;
  void visit(ConstDeclarationNode &const_declaration) override;
  void visit(DeclarationSequenceNode &declaration_sequence) override;
  void visit(ExpressionNode &expression) override;
  void visit(FPSectionNode &fp_section) override;
  void visit(IfStatementNode &if_statement) override;
  void visit(ModuleNode &module_node) override;
  void visit(ProcedureCallNode &procedure_call) override;
  void visit(ProcedureDeclarationNode &procedure_declaration) override;
  void visit(RecordTypeNode &record_type) override;
  void visit(RepeatStatementNode &repeat_statement) override;
  void visit(SelectorNode &selector) override;
  void visit(StatementNode &statement) override;
  void visit(StatementSequenceNode &statement_sequence) override;
  void visit(TypeNode &type) override;
  void visit(TypeDeclarationNode &type_declaration) override;
  void visit(VarDeclarationNode &var_declaration) override;
  void visit(WhileStatementNode &while_statement) override;
};

#endif // OBERON0C_NODEPRETTYPRINER_H
