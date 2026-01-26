#include "ASTContext.h"
#include "TypeNode.h"
#include "global.h"
#include <memory>

const StdType ASTContext::INTEGER = StdType("INTEGER");
const StdType ASTContext::BOOLEAN = StdType("BOOLEAN");

const std::shared_ptr<ProcedureTypeNode> ASTContext::STD_PROCEDURE_TYPE =
    std::make_shared<ProcedureTypeNode>(EMPTY_POS);

ModuleNode *ASTContext::get_module() { return module_.get(); }

void ASTContext::set_module(unique_ptr<ModuleNode> module) {
  module_ = std::move(module);
}

IdentTypeNode *ASTContext::add_type(unique_ptr<IdentTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
ArrayTypeNode *ASTContext::add_type(unique_ptr<ArrayTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
RecordTypeNode *ASTContext::add_type(unique_ptr<RecordTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
