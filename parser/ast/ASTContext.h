#ifndef OBERON0C_ASTCONTEXT_H
#define OBERON0C_ASTCONTEXT_H

#include "ModuleNode.h"
#include "functional"
#include "parser/ast/TypeNode.h"

using std::function;

class StdType {
private:
  const unique_ptr<IdentTypeNode> type_;
  unique_ptr<IdentTypeNode> std_ident_type_(string name) {
    return std::make_unique<IdentTypeNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, name));
  };

public:
  StdType(const string name) : type_(std_ident_type_(name)) {};
  ~StdType() = default;

  IdentTypeNode *get() const { return type_.get(); };
  const string get_name() const { return type_->ident->value; };
};

class ASTContext {
private:
  vector<unique_ptr<TypeNode>> types_;
  unique_ptr<ModuleNode> module_;

public:
  ASTContext() {}; // TODO add standard environment
  ~ASTContext() = default;
  static const StdType INTEGER;
  static const StdType BOOLEAN;

  // NOTE for multi-module use this should be a vector
  ModuleNode *get_module();
  void set_module(unique_ptr<ModuleNode>);

  IdentTypeNode *add_type(unique_ptr<IdentTypeNode>);
  ArrayTypeNode *add_type(unique_ptr<ArrayTypeNode>);
  RecordTypeNode *add_type(unique_ptr<RecordTypeNode>);
  ProcedureTypeNode *add_type(unique_ptr<ProcedureTypeNode>);
};

#endif
