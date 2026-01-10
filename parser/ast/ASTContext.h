#include "ModuleNode.h"

class ASTContext {
private:
  vector<unique_ptr<TypeNode>> types_;
  unique_ptr<ModuleNode> module_;

public:
  ASTContext() {}; // TODO add standard environment
  ~ASTContext() = default;

  // NOTE for multi-module use this should be a vector
  void set_module(unique_ptr<ModuleNode> module);

  void add_type(unique_ptr<TypeNode> type);
};
