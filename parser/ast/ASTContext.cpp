#include "ASTContext.h"

void ASTContext::set_module(unique_ptr<ModuleNode> module) {
  module_ = std::move(module);
}

void ASTContext::add_type(unique_ptr<TypeNode> type) {
  types_.push_back(std::move(type));
}
