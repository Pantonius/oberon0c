#include "ModuleNode.h"
#include "parser/ast/TypeNode.h"

class ASTContext {
private:
  vector<unique_ptr<TypeNode>> types_;
  unique_ptr<ModuleNode> module_;

public:
  ASTContext() {}; // TODO add standard environment
  ~ASTContext() = default;

  // NOTE for multi-module use this should be a vector
  ModuleNode *get_module();
  void set_module(unique_ptr<ModuleNode>);

  void add_type(unique_ptr<TypeNode>);

  TypeNode *find_type(IdentTypeNode *);
  TypeNode *find_type(ArrayTypeNode *);
  TypeNode *find_type(RecordTypeNode *);
};
