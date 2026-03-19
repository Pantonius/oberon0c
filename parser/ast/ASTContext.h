#ifndef OBERON0C_ASTCONTEXT_H
#define OBERON0C_ASTCONTEXT_H

#include "ModuleNode.h"
#include "functional"
#include "parser/ast/TypeNode.h"

using std::function;

enum class StdProc { WRITE_INT, WRITE_LN };

class ASTContext {
private:
  vector<unique_ptr<TypeNode>> types_;
  unique_ptr<ModuleNode> module_;

public:
  ASTContext() {}; // TODO add standard environment
  ~ASTContext() = default;
  static const std::unordered_map<std::string, StdTypeNode *const> std_types;
  static StdTypeNode *const BOOLEAN;
  static StdTypeNode *const INTEGER;

  static const std::unordered_map<StdProc, ProcedureDeclarationNode *const>
      std_procs;
  static ProcedureDeclarationNode *const WRITE_INT;
  static ProcedureDeclarationNode *const WRITE_LN;

  // NOTE for multi-module use this should be a vector
  ModuleNode *get_module();
  void set_module(unique_ptr<ModuleNode>);

  IdentTypeNode *add_type(unique_ptr<IdentTypeNode>);
  ArrayTypeNode *add_type(unique_ptr<ArrayTypeNode>);
  RecordTypeNode *add_type(unique_ptr<RecordTypeNode>);
  ProcedureTypeNode *add_type(unique_ptr<ProcedureTypeNode>);
};

#endif
