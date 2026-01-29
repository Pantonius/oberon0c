#ifndef OBERON0C_NODEVISITOR_H
#define OBERON0C_NODEVISITOR_H

#include "ModuleNode.h"

class NodeVisitor {

public:
  explicit NodeVisitor() = default;
  virtual ~NodeVisitor() noexcept;

  virtual void visit(ArrayTypeNode &) = 0;
  virtual void visit(AssignmentNode &) = 0;
  virtual void visit(ConstDeclarationNode &) = 0;
  virtual void visit(ExpressionNode &) = 0;
  virtual void visit(IfStatementNode &) = 0;
  virtual void visit(ModuleNode &) = 0;
  virtual void visit(ProcedureTypeNode &) = 0;
  virtual void visit(ProcedureCallNode &) = 0;
  virtual void visit(ProcedureDeclarationNode &) = 0;
  virtual void visit(RecordTypeNode &) = 0;
  virtual void visit(RepeatStatementNode &) = 0;
  virtual void visit(SelectorNode &) = 0;
  virtual void visit(StatementNode &) = 0;
  virtual void visit(StatementSequenceNode &) = 0;
  virtual void visit(IdentNode &) = 0;
  virtual void visit(FieldNode &) = 0;
  virtual void visit(TypeDeclarationNode &) = 0;
  virtual void visit(ParamDeclarationNode &) = 0;
  virtual void visit(VarDeclarationNode &) = 0;
  virtual void visit(WhileStatementNode &) = 0;
};

#endif // OBERON0C_NODEVISITOR_H
